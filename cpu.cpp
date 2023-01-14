#include <bitset>
#include <sstream>
#include "cpu.h"
#include "logger.h"

bool is_two_byte_instr(Byte b)
{
    Nibble opr = get_high_nibble_from_byte(b);
    Nibble opa = get_low_nibble(b);

    switch (opr)
    {
        case 1: // JCN
        case 4: // JUN
        case 5: // JMS
        case 7: // ISZ
            return true;
        case 2:
             if (opa % 2 == 0) return true; // FIM
             else return false; // SRC
        default:
            return false;
    }
}

void CPU4004::run()
{
    while(true)
    {
        auto [b1,b2] = read_instr();
        int this_ip = ip;
        ip++;
        if (is_two_byte_instr(b1)) ip++;
        exec_instr(b1,b2);

#ifdef DEBUG
        // Avoid printing busy waits by checking if ip repeats
        if (ip != this_ip)
        {
          std::ostringstream oss;
          oss << "ip " << this_ip << " " << std::bitset<8>(b1) << " " << std::bitset<8>(b2);
          send_to_log(oss.str());
        }
#endif

    }
}

std::pair<Byte,Byte> CPU4004::read_instr()
{
    Nibble rom_num  = get_high_nibble_from_addr(ip);
    Byte   rom_addr = get_low_byte(ip);
    // TODO: Figure out how to properly handle a 2-byte instruction at the end
    // of a ROM. (It's probably not supported, so not too important.)
    return {roms->at(rom_num).read(rom_addr),
            roms->at(rom_num).read((rom_addr+1) % 256)};
}

void CPU4004::exec_instr(Byte b1, Byte b2)
{
    Nibble low_nibble = get_low_nibble(b1);

    // NOP
    if (b1 == 0b00000000) {}

    else if (b1 <  0b00010000) throw InvalidInstr(); 

    // Conditional jumps
    else if (b1 <  0b00100000)
    {
        switch (low_nibble)
        {
            case 0b0001:
                if (!tdev->test()) ip = b2; break;
            case 0b0010:
                if (carry)  ip = b2; break;
            case 0b0100:
                if (!acc)   ip = b2; break;
            case 0b1001:
                if (tdev->test()) ip = b2; break;
            case 0b1010:
                if (!carry) ip = b2; break;
            case 0b1100:
                if (acc)    ip = b2; break;
            default:
                throw InvalidInstr();
        }
    }

    // FIM
    else if (b1 < 0b00110000 && is_even(b1))
    {
        assert(false);
    }

    // SRC
    else if (b1 < 0b00110000 && is_odd(b1))
    {
        assert(false);
    }

    // FIN
    else if (b1 < 0b01000000 && is_even(b1))
    {
        assert(false);
    }

    // JIN
    else if (b1 < 0b01000000 && is_odd(b1))
    {
        assert(false);
    }

    // JUN
    else if (b1 < 0b01010000)
    {
        ip = bytes_to_addr(b1,b2);
    }

    // JMS
    else if (b1 < 0b01100000)
    {
        assert(false);
    }

    // INC
    else if (b1 < 0b01110000)
    {
        Nibble reg_num = get_low_nibble(b1);
        reg[reg_num] = INC(reg[reg_num]);
    }

    // ADD CY
    else if (b1 < 0b10010000)
    {
        Bit carry_out = 0;
        Nibble reg_num = get_low_nibble(b1);
        acc = ADD(acc, reg[reg_num], carry, carry_out);
        carry = carry_out;
    }

    // SUB CY
    else if (b1 < 0b10100000)
    {
        Bit borrow = 0;
        Nibble reg_num = get_low_nibble(b1);
        acc = SUB(acc, reg[reg_num], carry, borrow);
        carry = borrow;
    }

    // LD
    else if (b1 < 0b10110000)
    {
        Nibble reg_num = get_low_nibble(b1);
        acc = reg[reg_num];
    }

    // XCH
    else if (b1 < 0b11000000)
    {
        Nibble reg_num = get_low_nibble(b1);
        Nibble tmp = acc;
        acc = reg[reg_num];
        reg[reg_num] = tmp;
    }

    // BBL
    else if (b1 < 0b11010000)
    {
        assert(false);
    }

    // LDM
    else if (b1 < 0b11100000)
    {
        acc = get_low_nibble(b1);
    }

    // All instructions >= 0b11100000)
    else
    {

    switch(b1)
    {
        // WRM
        case 0b11100000:
            assert(false);
            break;
        // WMP
        case 0b11100001:
            rams->at(sel_ram).port_output(acc);
            break;
        // WRR
        case 0b11100010:
            assert(false);
            break;
        // WPM
        case 0b11100011:
            assert(false);
            break;
        // WR0
        case 0b11100100:
            assert(false);
            break;
        // WR1
        case 0b11100101:
            assert(false);
            break;
        // WR2
        case 0b11100110:
            assert(false);
            break;
        // WR3
        case 0b11100111:
            assert(false);
            break;
        // SBM
        case 0b11101000:
            assert(false);
            break;
        // RDM
        case 0b11101001:
            assert(false);
            break;
        // RDR
        case 0b11101010:
            acc = roms->at(sel_rom).port_input();
            break;
        // ADM
        case 0b11101011:
            assert(false);
            break;
        // RD0
        case 0b11101100:
            assert(false);
            break;
        // RD1
        case 0b11101101:
            assert(false);
            break;
        // RD2
        case 0b11101110:
            assert(false);
            break;
        // RD3
        case 0b11101111:
            assert(false);
            break;
        // CLB
        case 0b11110000:
            acc   = 0;
            carry = 0;
            break;
        // CLC
        case 0b11110001:
            carry = 0;
            break;
        // IAC
        case 0b11110010:
            acc = INC(acc);
            break;
        // CMC
        case 0b11110011:
            assert(false);
            break;
        // CMA
        case 0b11110100:
            assert(false);
            break;
        // RAL
        case 0b11110101:
        {
            Bit carry_out = 0;
            acc = RAL(acc, carry, carry_out);
            carry = carry_out;
            break;
        }
        // RAR
        case 0b11110110:
        {
            Bit carry_out = 0;
            acc = RAR(acc, carry, carry_out);
            carry = carry_out;
            break;
        }
        // TCC
        case 0b11110111:
            assert(false);
            break;
        // DAC
        case 0b11111000:
            assert(false);
            break;
        // TCS
        case 0b11111001:
            assert(false);
            break;
        // STC
        case 0b11111010:
            carry = 1;
            break;
        // DAA
        case 0b11111011:
            assert(false);
            break;
        // KBP
        case 0b11111100:
            assert(false);
            break;
        // DCL
        case 0b11111101:
            assert(false);
            break;
        // WRM
        default:
            throw InvalidInstr();
    }

    }
}

