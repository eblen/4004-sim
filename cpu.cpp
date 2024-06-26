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
        int this_ip = ip[current_ip];
        ip[current_ip]++;
        if (is_two_byte_instr(b1)) ip[current_ip]++;
        exec_instr(b1,b2);

#ifdef DEBUG
        // Avoid printing busy waits by checking if ip repeats
        if (ip[current_ip] != this_ip)
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
    Nibble rom_num  = get_high_nibble_from_addr(ip[current_ip]);
    Byte   rom_addr = get_low_byte(ip[current_ip]);
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
                if (!tdev->test()) ip[current_ip] = b2; break;
            case 0b0010:
                if (carry)  ip[current_ip] = b2; break;
            case 0b0100:
                if (!acc)   ip[current_ip] = b2; break;
            case 0b1001:
                if (tdev->test()) ip[current_ip] = b2; break;
            case 0b1010:
                if (!carry) ip[current_ip] = b2; break;
            case 0b1100:
                if (acc)    ip[current_ip] = b2; break;
            default:
                throw InvalidInstr();
        }
    }

    // FIM
    else if (b1 < 0b00110000 && is_even(b1))
    {
        Byte rom_val = roms->at(sel_rom).read(b2);

        Nibble dest_reg = (get_low_nibble(b1) >> 1) * 2;
        reg[dest_reg]   = get_high_nibble_from_byte(rom_val);
        reg[dest_reg+1] = get_low_nibble(rom_val);
    }

    // SRC
    else if (b1 < 0b00110000 && is_odd(b1))
    {
        Nibble src_reg = (get_low_nibble(b1) >> 1) * 2;
        sel_rom     = reg[src_reg];
        sel_ram     = reg[src_reg] >> 2;
        sel_ram_reg = reg[src_reg] & 0b0011;
        sel_ram_mmc = reg[src_reg+1];
    }

    // FIN
    else if (b1 < 0b01000000 && is_even(b1))
    {
        Byte rom_addr = nibbles_to_byte(reg[0], reg[1]);
        Byte rom_val  = roms->at(sel_rom).read(rom_addr);

        Nibble dest_reg = (get_low_nibble(b1) >> 1) * 2;
        reg[dest_reg]   = get_high_nibble_from_byte(rom_val);
        reg[dest_reg+1] = get_low_nibble(rom_val);
    }

    // JIN
    else if (b1 < 0b01000000 && is_odd(b1))
    {
        Nibble dest_reg = (get_low_nibble(b1) >> 1) * 2;
        Byte local_rom_addr = nibbles_to_byte(reg[dest_reg], reg[dest_reg+1]);

        Nibble current_rom = get_high_nibble_from_addr(ip[current_ip]);
        ip[current_ip] = bytes_to_addr(current_rom, local_rom_addr);
    }

    // JUN
    else if (b1 < 0b01010000)
    {
        ip[current_ip] = bytes_to_addr(b1,b2);
    }

    // JMS
    else if (b1 < 0b01100000)
    {
        // This is actually allowed according to the MCS manual but leads to
        // the deepest return address being overwritten! (current_ip is mod 4).
        assert(current_ip < 3);

        current_ip++;
        ip[current_ip] = bytes_to_addr(b1,b2);
    }

    // INC
    else if (b1 < 0b01110000)
    {
        Nibble reg_num = get_low_nibble(b1);
        reg[reg_num] = INC(reg[reg_num]);
    }

    // ISZ
    else if (b1 < 0b10000000)
    {
        Nibble reg_num = get_low_nibble(b1);
        reg[reg_num] = INC(reg[reg_num]);

        Nibble current_rom = get_high_nibble_from_addr(ip[current_ip]);
        if (reg[reg_num] != 0) ip[current_ip] = bytes_to_addr(current_rom,b2);
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
        // This is allowed according to the MCS manual (see JMS comments) but
        // is either a mistake or means return addresses have been overwritten.
        assert(current_ip > 0);

        current_ip--;
        acc = get_low_nibble(b1);
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
            rams->at(sel_ram).write_mmc(sel_ram_reg, sel_ram_mmc, acc);
            break;
        // WMP
        case 0b11100001:
            rams->at(sel_ram).port_output(acc);
            break;
        // WRR
        case 0b11100010:
            roms->at(sel_rom).port_output(acc);
            break;
        // WPM
        case 0b11100011:
            // Only used on 4008/4009
            assert(false);
            break;
        // WR0
        case 0b11100100:
            rams->at(sel_ram).write_stat(sel_ram_reg, 0, acc);
            break;
        // WR1
        case 0b11100101:
            rams->at(sel_ram).write_stat(sel_ram_reg, 1, acc);
            break;
        // WR2
        case 0b11100110:
            rams->at(sel_ram).write_stat(sel_ram_reg, 2, acc);
            break;
        // WR3
        case 0b11100111:
            rams->at(sel_ram).write_stat(sel_ram_reg, 3, acc);
            break;
        // SBM
        case 0b11101000:
        {
            Bit borrow = 0;
            Nibble mmc = rams->at(sel_ram).read_mmc(sel_ram_reg, sel_ram_mmc);
            acc = SUB(acc, mmc, carry, borrow);
            carry = borrow;
            break;
        }
        // RDM
        case 0b11101001:
            acc = rams->at(sel_ram).read_mmc(sel_ram_reg, sel_ram_mmc);
            break;
        // RDR
        case 0b11101010:
            acc = roms->at(sel_rom).port_input();
            break;
        // ADM
        case 0b11101011:
        {
            Bit carry_out = 0;
            Nibble mmc = rams->at(sel_ram).read_mmc(sel_ram_reg, sel_ram_mmc);
            acc = ADD(acc, mmc, carry, carry_out);
            carry = carry_out;
            break;
        }
        // RD0
        case 0b11101100:
            acc = rams->at(sel_ram).read_stat(sel_ram_reg, 0);
            break;
        // RD1
        case 0b11101101:
            acc = rams->at(sel_ram).read_stat(sel_ram_reg, 1);
            break;
        // RD2
        case 0b11101110:
            acc = rams->at(sel_ram).read_stat(sel_ram_reg, 2);
            break;
        // RD3
        case 0b11101111:
            acc = rams->at(sel_ram).read_stat(sel_ram_reg, 3);
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
            carry = (carry == 0) ? 1 : 0;
            break;
        // CMA
        case 0b11110100:
            acc = COMPLEMENT(acc);
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
            acc = carry;
            carry = 0;
            break;
        // DAC
        case 0b11111000:
            acc = DEC(acc);
            break;
        // TCS
        case 0b11111001:
            acc = (carry == 0) ? 9 : 10;
            carry = 0;
            break;
        // STC
        case 0b11111010:
            carry = 1;
            break;
        // DAA
        case 0b11111011:
            if ((carry == 1) || (acc > 9))
            {
                Bit carry_out = 0;
                // Careful: this addition does not use the carry, and also it
                // sets the carry ONLY if there was a carry...
                acc = ADD(acc, 6, 0, carry_out);
                if (carry_out == 1) carry = 1;
            }
            break;
        // KBP
        case 0b11111100:
            switch (acc)
            {
                case 0:  acc =  0; break;
                case 1:  acc =  1; break;
                case 2:  acc =  2; break;
                case 4:  acc =  3; break;
                case 8:  acc =  4; break;
                default: acc = 15;
            }
            break;
        // DCL
        case 0b11111101:
            assert(false);
            break;
        default:
            throw InvalidInstr();
    }

    }
}

