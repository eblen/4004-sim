#include <map>
#include <string>
#include <vector>

#include "assembly_lib.h"

const std::map<std::string, std::string> opcode_to_binary{
    {"NOP", "00000000"}, {"JCN", "0001"}, {"FIM", "0010"}, {"SRC", "0010"}, {"FIN", "0011"}, {"JIN", "0011"},
    {"JUN", "0100"},     {"JMS", "0101"}, {"INC", "0110"}, {"ISZ", "0111"}, {"ADD", "1000"}, {"SUB", "1001"},
    {"LD",  "1010"},     {"XCH", "1011"}, {"BBL", "1100"}, {"LDM", "1101"},

    {"WRM", "11100000"}, {"WMP", "11100001"}, {"WRR", "11100010"}, {"WPM", "11100011"}, {"WR0", "11100100"}, 
    {"WR1", "11100101"}, {"WR2", "11100110"}, {"WR3", "11100111"}, {"SBM", "11101000"}, {"RDM", "11101001"}, 
    {"RDR", "11101010"}, {"ADM", "11101011"}, {"RD0", "11101100"}, {"RD1", "11101101"}, {"RD2", "11101110"}, 
    {"RD3", "11101111"},

    {"CLB", "11110000"}, {"CLC", "11110001"}, {"IAC", "11110010"}, {"CMC", "11110011"}, {"CMA", "11110100"}, 
    {"RAL", "11110101"}, {"RAR", "11110110"}, {"TCC", "11110111"}, {"DAC", "11111000"}, {"TCS", "11111001"}, 
    {"STC", "11111010"}, {"DAA", "11111011"}, {"KBP", "11111100"}, {"DCL", "11111101"}
};

// Get binary string representation (ebin) for the last "num_bits" of "num"
std::string uint_to_ebin(unsigned int num, int num_bits)
{
    std::string ret_val(num_bits, '0');
    for (int i=num_bits-1; i >= 0; i--)
    {
        if ((num & 1) == 1) ret_val[i] = '1';
        num >>= 1;
    }
    return ret_val;
}

// Return the instruction size (either 1 or 2 bytes) for the given mnemonic
// If not found, return 0
unsigned int instr_size(const std::string &orig_op)
{
    // Convert op to uppercase
    std::string op(orig_op);
    transform(op.begin(), op.end(), op.begin(), toupper);

    try
    {
        opcode_to_binary.at(op);
    }
    catch(std::out_of_range&)
    {
        return 0;
    }

    if ((op == "JCN") || (op == "FIM") || (op == "JUN") || (op == "JMS") || (op == "ISZ"))
    {
        return 2;
    }
    else {return 1;}
}

// Convert a single assembly instruction to ebin
std::pair<std::string, PARSE_ERROR> assembly_to_ebin(const std::string &orig_op, const std::vector<unsigned int> &orig_args)
{
    // No instruction takes more than two arguments
    if (orig_args.size() > 2) return std::pair("", TOO_MANY_ARGS);

    // Convert op to uppercase
    std::string op(orig_op);
    transform(op.begin(), op.end(), op.begin(), toupper);

    // Copy arguments so they can be modified
    std::vector<unsigned int> args(orig_args);

    // Get binary code for the op
    std::string ebin = "";
    try
    {
        ebin = opcode_to_binary.at(op);
    }
    catch(std::out_of_range&)
    {
        return std::pair("", BAD_MNEMONIC);
    }

    // Some instructions take a 3-bit argument (an index register pair), which
    // is converted to a nibble by appending a 0 or 1. Go ahead and do this so
    // that these instructions do not have to be handled as a special case.
    if ((op == "FIM") || (op == "SRC") || (op == "FIN") || (op == "JIN"))
    {
        if (args.empty()) return std::pair("", TOO_FEW_ARGS);
        args[0] <<= 1;
        if ((op == "SRC") || (op == "JIN")) args[0] += 1;
    }

    // For NOP or all instructions >= 11100000, no more is needed.
    // Return an error if user included any arguments.
    if ((op == "NOP") ||
       ((ebin[0] == '1') && (ebin[1] == '1') && (ebin[2] == '1')))
    {
        if (!args.empty()) return std::pair("", TOO_MANY_ARGS);
        else return std::pair(ebin, NONE);
    }

    // For all remaining instructions >= 10000000, and some others, only a
    // single nibble argument is needed.
    if ((op == "INC") || (op == "SRC") || (op == "FIN") || (op == "JIN") || (ebin[0] == '1'))
    {
        if (args.size() < 1) return std::pair("", TOO_FEW_ARGS);
        if (args.size() > 1) return std::pair("", TOO_MANY_ARGS);
        if (args[0] > 15)   return std::pair("", ARG1_OUT_OF_RANGE);

        return std::pair(ebin + " " + uint_to_ebin(args[0], 4), NONE);
    }

    // Three instructions take a nibble argument followed by a byte
    if ((op == "FIM") || (op == "JCN") || (op == "ISZ"))
    {
        if (args.size() < 2) return std::pair("", TOO_FEW_ARGS);
        if (args[0] >  15)  return std::pair("", ARG1_OUT_OF_RANGE);
        if (args[1] > 255)  return std::pair("", ARG2_OUT_OF_RANGE);

        return std::pair(ebin + " " + uint_to_ebin(args[0], 4) + " " + uint_to_ebin(args[1], 8), NONE);
    }

    // Finally, two instructions take a single 12-bit address
    if ((op == "JUN") || (op == "JMS"))
    {
        if (args.size() < 1) return std::pair("", TOO_FEW_ARGS);
        if (args.size() > 1) return std::pair("", TOO_MANY_ARGS);
        if (args[0] > 4095) return std::pair("", ARG1_OUT_OF_RANGE);

        return std::pair(ebin + " " + uint_to_ebin(args[0], 12), NONE);
    }

    // This point should never be reached
    assert(false);
}
