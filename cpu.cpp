#include "cpu.h"

void CPU4004::run()
{
    while(true)
    {
        auto [b1,b2] = read_instr();
        exec_instr(b1,b2);
    }
}

std::tuple<Byte,Byte> CPU4004::read_instr()
{
    Nibble rom_num = get_high_nibble(ip);
    return {roms->at(rom_num).read(get_low_byte(ip)),
            roms->at(rom_num).read(get_next_to_low_byte(ip))};
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
    }

    // SRC
    else if (b1 < 0b00110000 && is_odd(b1))
    {
    }

    // FIN
    else if (b1 < 0b01000000 && is_even(b1))
    {
    }

    // JIN
    else if (b1 < 0b01000000 && is_odd(b1))
    {
    }

    // JUN
    else if (b1 < 0b01010000)
    {
    }

    // JMS
    else if (b1 < 0b01100000)
    {
    }

    // INC
    else if (b1 < 0b01110000)
    {
    }

    // ADD CY
    else if (b1 < 0b10010000)
    {
    }

    // SUB CY
    else if (b1 < 0b10100000)
    {
    }

    // LD
    else if (b1 < 0b10110000)
    {
    }

    // XCH
    else if (b1 < 0b11000000)
    {
    }

    // BBL
    else if (b1 < 0b11010000)
    {
    }

    // LDM
    else if (b1 < 0b11100000)
    {
    }

    // All instructions >= 0b11100000)
    else
    {

    switch(b1)
    {
        // WRM
        case 0b11100000:
            break;
        // WMP
        case 0b11100001:
            break;
        // WRR
        case 0b11100010:
            break;
        // WPM
        case 0b11100011:
            break;
        // WR0
        case 0b11100100:
            break;
        // WR1
        case 0b11100101:
            break;
        // WR2
        case 0b11100110:
            break;
        // WR3
        case 0b11100111:
            break;
        // SBM
        case 0b11101000:
            break;
        // RDM
        case 0b11101001:
            break;
        // RDR
        case 0b11101010:
            break;
        // ADM
        case 0b11101011:
            break;
        // RD0
        case 0b11101100:
            break;
        // RD1
        case 0b11101101:
            break;
        // RD2
        case 0b11101110:
            break;
        // RD3
        case 0b11101111:
            break;
        // CLB
        case 0b11110000:
            break;
        // CLC
        case 0b11110001:
            break;
        // IAC
        case 0b11110010:
            break;
        // CMC
        case 0b11110011:
            break;
        // CMA
        case 0b11110100:
            break;
        // RAL
        case 0b11110101:
            break;
        // RAR
        case 0b11110110:
            break;
        // TCC
        case 0b11110111:
            break;
        // DAC
        case 0b11111000:
            break;
        // TCS
        case 0b11111001:
            break;
        // STC
        case 0b11111010:
            break;
        // DAA
        case 0b11111011:
            break;
        // KBP
        case 0b11111100:
            break;
        // DCL
        case 0b11111101:
            break;
        // WRM
        default:
            throw InvalidInstr();
    }

    }
}
