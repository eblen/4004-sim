#include "ncurses.h"

#include "types.h"
#include "iodevice.h"

// A simple output device for printing hexadecimal digits

// To support formatting (spaces and newlines), two nibbles
// must be provided for each output, a "character set" and
// then the actual character. The "SPACES" character set
// currently only supports spaces and newlines.

// The device keeps track of how many bits for the output
// have been set (ports 0-3) and only prints once four of
// them are set. This should work fine as long as the CPU
// writes bytes to ports 0-3 in a single write operation.
// Writing bits individually may create unexpected results.

// Map of input values
// character set 0: SPACES (0:space, 1:newline)
// character set 1: DIGITS (hexadecimal digit)
class Hextape : public iodevice
{
    enum input_mode {CHAR_SET, CHARS};
    enum char_set   {SPACES=0, DIGITS=1};

    public:
    Hextape() :imode(CHAR_SET), input_bits(0), num_bits_set(0), cset(SPACES)
    {
        initscr();
        cbreak();
        noecho();
    }

    // No input capability
    Bit port_input(int port_id) {return 0;}

    void port_output(int port_id, Bit val)
    {
        // Just set bits of output until four are set
        assert((port_id >= 0) && (port_id <= 3));
        input_bits = set_bit(input_bits, port_id, val);
        num_bits_set++;
        if (num_bits_set < 4) return;
        else num_bits_set = 0;

        // Four bits written, so assume we are ready to print

        // For "character set" mode, just store the character set value
        if (imode == CHAR_SET)
        {
            if (input_bits == 0 || input_bits == 1) cset = char_set(input_bits);
            else assert(false);
        }

        // Otherwise, print characters from the proper character set.

        // Print space character for "SPACES" character set
        else if (cset == SPACES)
        {
            if (input_bits == 0) printw("%c",' ');
            else if (input_bits == 1) printw("%c",'\n');
            else assert(false);
            refresh();
        }

        // Print hex digit for "DIGITS" character set
        else if (cset == DIGITS)
        {
            printw("%c", nibble_numeric_value_to_char(input_bits));
            refresh();
        }

        // Invalid character set
        // Should never happen since we check when assigning.
        else assert(false);

        // Input mode just alternates with each input.
        imode = (imode == CHAR_SET) ? CHARS : CHAR_SET;
    }

    private:
    input_mode imode;
    Nibble     input_bits;
    int        num_bits_set;
    char_set   cset;
};
