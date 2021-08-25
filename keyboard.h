#include <mutex>
#include "ncurses.h"

#include "types.h"
#include "iodevice.h"

// A simple input device for debugging
class Keyboard : public iodevice, public testdevice
{
    public:
    Keyboard() :buffer{0,0}, num_input_nibbles(0) {}

    // Input one nibble at a time upon request.
    Nibble port_input()
    {
        std::lock_guard<std::mutex> l(kb_buffer_mutex);
        Nibble ret_val = buffer[0];
        buffer[0] = buffer[1];
        buffer[1] = 0;
        num_input_nibbles--;
        return ret_val;
    }

    // No output capability
    void port_output(Nibble val) {}

    // Indicates whether there are nibbles to be read.
    // (User must keep track of whether nibble is high or low.)
    Bit test() {
        std::lock_guard<std::mutex> l(kb_buffer_mutex);
        return (num_input_nibbles != 0);
    }

    // User can input a single char at a time, which is broken into 2 nibbles.
    void read_char(char c)
    {
        std::lock_guard<std::mutex> l(kb_buffer_mutex);
        // A primitive device that does not store more than one byte at a time for output.
        if (num_input_nibbles > 0) return;

        Byte b = static_cast<Byte>(c);
        buffer[0] = get_high_nibble_from_byte(b);
        buffer[1] = get_low_nibble(b);
        num_input_nibbles = 2;
    }

    void run()
    {
        initscr();
        while(true)
        {
            // TODO: Figure out why "flushinp" is needed.
            // (Otherwise, there is a single character delay.)
            flushinp();
            read_char(getch());
        }
    }

    private:
    Nibble buffer[2];
    Crumb num_input_nibbles;
    std::mutex kb_buffer_mutex;
};
