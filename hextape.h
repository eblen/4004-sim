#include "ncurses.h"

#include "types.h"
#include "iodevice.h"

// A simple output device for printing hexadecimal digits

// To support formatting (spaces and newlines), two nibbles
// must be provided for each output, a "character set" and
// then the actual character. The "SPACES" character set
// currently only supports spaces and newlines.

// Map of input values
// character set 0: SPACES (0:space, 1:newline)
// character set 1: DIGITS (hexadecimal digit)
class Hextape : public iodevice
{
    enum input_mode {CHAR_SET, CHARS};
    enum char_set   {SPACES=0, DIGITS=1};

    public:
    Hextape() :imode(CHAR_SET), cset(SPACES)
    {
        initscr();
        cbreak();
        noecho();
    }

    // No input capability
    Nibble port_input() {return 0;}

    void port_output(Nibble val)
    {
       // For "character set" mode, just store the character set value
       if (imode == CHAR_SET)
       {
           if (val == 0 || val == 1) cset = char_set(val);
           else assert(false);
       }

       // Otherwise, print characters from the proper character set.

       // Print space character for "SPACES" character set
       else if (cset == SPACES)
       {
           if (val == 0) printw("%c",' ');
           else if (val == 1) printw("%c",'\n');
           else assert(false);
           refresh();
       }

       // Print hex digit for "DIGITS" character set
       else if (cset == DIGITS)
       {
           printw("%c", nibble_numeric_value_to_char(val));
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
    char_set   cset;
};
