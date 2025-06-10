#include "ncurses.h"

#include "types.h"
#include "iodevice.h"

// A simple output device for debugging

// The device keeps track of how many bits for the output
// have been set (ports 0-3) and only proceeds once four of
// them are set. This should work fine as long as the CPU
// writes bytes to ports 0-3 in a single write operation.
// Writing bits individually may create unexpected results.
class Tape : public iodevice
{
    public:
    Tape() :buffer_idx(false), buffer{0,0}, num_bits_set(0)
    {
        initscr();
        cbreak();
        noecho();
    }

    // No input capability
    Bit port_input(int port_id) {return 0;}

    // Combine every 2 incoming nibbles and print the corresponding character.
    void port_output(int port_id, Bit val)
    {
        // Just set bits of output until four are set before proceeding with
        // byte-level logic
        assert((port_id >= 0) && (port_id <= 3));
        buffer[buffer_idx] = set_bit(buffer[buffer_idx], port_id, val);
        num_bits_set++;
        if (num_bits_set < 4) return;
        else num_bits_set = 0;

       if (buffer_idx == 1)
       {
           char c = byte_to_char(nibbles_to_byte(buffer[0], buffer[1]));
           printw("%c",c);
           refresh();
       }

       buffer_idx = buffer_idx == 0 ? 1:0;
    }

    private:
    bool buffer_idx;
    Nibble buffer[2];
    int num_bits_set;
};
