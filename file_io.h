#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include "types.h"

std::vector<Byte> ebin_to_bytes(std::string input_file)
{
    // Open input file
    std::ifstream ifile;
    ifile.open(input_file);
    if (ifile.fail())
    {
        throw std::runtime_error("Unable to open ebin file");
    }

    // First, read in all 0 and 1 characters and store as one long vector
    std::vector<char> ebytes;
    char c;
    bool ignore_input = false;
    while (ifile.get(c))
    {
        switch(c)
        {
            case '0':
            case '1':
                if (!ignore_input) ebytes.push_back(c);
                break;
            case ';':
                ignore_input = true;
                break;
            case '\n':
                ignore_input = false;
                break;
            // Ignore spaces
            case ' ':
                break;
            default:
                if (!ignore_input)
                {
                    throw std::runtime_error("Found uncommented non-binary character while parsing ebin file");
                }
        }
    }
    ifile.close();

    // Second, convert vector of characters into vector of bytes
    std::vector<Byte> bytes;
    int counter = 0;
    Byte b = 0;
    for (char c : ebytes)
    {
       b <<= 1;
       if (c == '1') b |= 1;

       counter++;
       if (counter == 8)
       {
           bytes.push_back(b); 
           counter = 0;
           b = 0;
       }    
    }

    // Pad any last, incomplete byte with zeros and insert it
    if (counter > 0)
    {
        b <<= (8 - counter);
        bytes.push_back(b);
    }

    return bytes;
}
