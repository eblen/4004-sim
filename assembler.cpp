#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "assembly_lib.h"

int main(int argc, char** argv)
{
    const std::map<char,int> symbol_to_num_base{ {'%', 2}, {'#', 10}, {'$', 16} };
    std::map<std::string, unsigned int> label_to_address;
    std::vector<std::string> lines_of_assembly;

    // First pass is simply for storing labels
    int line_num = 0;
    int byte_num = 0;
    for (std::string line; std::getline(std::cin, line);)
    {
        line_num++;
        lines_of_assembly.push_back(line);

        // Skip commented or empty lines
        if ((line[0] == ';') || (line.size() == 0)) continue;

        std::string op;
        std::istringstream iss(line);
        // op is all we need for the first pass
        iss >> op;

        if (op[0] == ':')
        {
            op.erase(op.begin());
            label_to_address[op] = byte_num;
        }

        else
        {
            int isize = instr_size(op);
            if (isize == 0)
            {
                std::cerr << "Error on line " << line_num << ": unrecognized instruction" << std::endl;
                exit(0);
            }
            byte_num += isize;
        }
    }

    // Second pass does the bulk of the work
    line_num = 0;
    byte_num = 0;
    for (std::string line : lines_of_assembly)
    {
        line_num++;

        // Skip commented or empty lines
        if ((line[0] == ';') || (line.size() == 0)) continue;

        // Remove commented portions
        size_t comm_start = line.find_first_of(';');
        if (comm_start != std::string::npos) line.erase(comm_start);

        std::string op;
        std::vector<unsigned int> args;
        std::istringstream iss(line);

        //  First, read in the "op" or operation mnemonic
        iss >> op;

        // If this is a label, record it and skip to the next line
        if (op[0] == ':')
        {
            op.erase(op.begin());
            label_to_address[op] = byte_num;
            continue;
        }

        // Now read in numeric arguments
        std::string arg_input;
        iss >> arg_input;
        while (iss)
        {
            // If a label, pull the address from storage and add it as an argument
            if (arg_input[0] == ':')
            {
                arg_input.erase(arg_input.begin());
                unsigned int addr;
                try
                {
                    addr = label_to_address.at(arg_input);
                }
                catch(std::out_of_range&)
                {
                    std::cerr << "Error on line " << line_num << ": Unrecognized label" << std::endl;
                    exit(0);
                }
                
                args.push_back(addr);
            }

            // Otherwise, read as a number
            else
            {
                int num_base;
                try
                {
                    num_base = symbol_to_num_base.at(arg_input[0]);
                }
                catch(const std::out_of_range& e)
                {
                    std::cerr << "Error on line " << line_num << ": Numbers must begin with %, #, or $" << std::endl;
                    exit(0);
                }

                // Convert string to integer and add it as an argument
                arg_input.erase(arg_input.begin());
                args.push_back(std::stoi(arg_input, nullptr, num_base));
            }

            iss >> arg_input;
        }

        // Now we are ready to convert the assembly to ebin
        auto [ebin, p_error] = assembly_to_ebin(op, args);

        if (p_error == NONE) std::cout << ebin << std::endl;
        else
        {
            std::cerr << "Error on line " << line_num << ": ";
            switch (p_error)
            {
                case BAD_MNEMONIC:
                    std::cerr << "unrecognized instruction";
                    break;
                case TOO_FEW_ARGS:
                    std::cerr << "not enough arguments";
                    break;
                case TOO_MANY_ARGS:
                    std::cerr << "too many arguments";
                    break;
                case ARG1_OUT_OF_RANGE:
                    std::cerr << "first argument out of range";
                    break;
                case ARG2_OUT_OF_RANGE:
                    std::cerr << "second argument out of range";
                    break;
                default:
                    assert(false);
            }
            std::cerr << std::endl;
            exit(0);
        }

        int isize = instr_size(op);
        // Should never happen, because we already checked that op was valid.
        if (isize == 0) assert(false);
        byte_num += isize;
    }
    return 0;
}
