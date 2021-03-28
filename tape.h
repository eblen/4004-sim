#include "ncurses.h"

#include "types.h"
#include "iodevice.h"

// A simple output device for debugging
class Tape : public iodevice
{
    public:
    Tape() :print_byte(false), buffer(0)
    {
        initscr();
        noecho();
    }

    // No input capability
    Nibble port_input() {return 0;}

    // Combine every 2 incoming nibbles and print the corresponding character.
    void port_output(Nibble val)
    {
       if (print_byte)
       {
           char c = byte_to_char(nibbles_to_byte(buffer, val));
           printw("%c",c);
       }
       else buffer = val;
       print_byte = !print_byte;
    }

    private:
    bool print_byte;
    Nibble buffer;
};
