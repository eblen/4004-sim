#include <mutex>
#include "ncurses.h"

#include "types.h"
#include "iodevice.h"

// A simple input device for debugging
class Keyboard : public iodevice, public testdevice
{
    public:
    Keyboard() :buffer{0,0}, bit_read_status(0), num_input_nibbles(0) {}

    // Input one nibble at a time upon request.
    Bit port_input(int port_id)
    {
        assert((port_id >= 0) && (port_id <= 3));
        // Avoid by only calling when "test" returns true
        assert(num_input_nibbles > 0);

        std::lock_guard<std::mutex> l(kb_buffer_mutex);

        // Indicates bit was read twice for the same nibble
        if (get_bit(bit_read_status, port_id) == 1) assert(false);

        bit_read_status = set_bit(bit_read_status, port_id, 1);
        Nibble ret_val = get_bit(buffer[0], port_id);

        // All bits read for the current nibble. Reset for the next.
        if (bit_read_status == 15)
        {
            buffer[0] = buffer[1];
            buffer[1] = 0;
            bit_read_status = 0;
            num_input_nibbles--;
        }

        return ret_val;
    }

    // No output capability
    void port_output(int port_id, Bit val) {}

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
        while(true) read_char(getch());
    }

    private:
    Nibble buffer[2];
    Nibble bit_read_status;
    Crumb num_input_nibbles;
    std::mutex kb_buffer_mutex;
};
