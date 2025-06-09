#include <mutex>
#include "ncurses.h"

#include "types.h"
#include "iodevice.h"

// A simple input device for inputting hexadecimal digits
// Ports 0-3 report user input (a nibble)
// The test input indicates when new user input is available.
//
// Device clears test input when one of the ports is read.
// Since this allows for new user input, fast typing could
// cause garbled input. Ideally, it would only clear after
// all ports are read, but this is just an example device.
class Hexpad: public iodevice, public testdevice
{
    public:
    Hexpad() :user_input(0), input_ready(false) {}

    // Input one bit at a time upon request.
    Bit port_input(int port_id)
    {
        assert((port_id >= 0) && (port_id <= 3));
        std::lock_guard<std::mutex> l(kb_buffer_mutex);
        input_ready = false;
        return get_bit(user_input, port_id);
    }

    // No output capability
    void port_output(int port_id, Bit val) {}

    // Indicates whether input is available.
    Bit test() {
        std::lock_guard<std::mutex> l(kb_buffer_mutex);
        return input_ready;
    }

    void read_char(char c)
    {
        std::lock_guard<std::mutex> l(kb_buffer_mutex);
        // Only store one Nibble at a time for output
        if (input_ready) return;

        auto [val, is_valid_hex_digit] = char_to_nibble_numeric_value(c);
        if (is_valid_hex_digit)
        {
            user_input = val;
            input_ready = true; 
        }
    }

    void run()
    {
        while(true) read_char(getch());
    }

    private:
    std::mutex kb_buffer_mutex;
    Nibble user_input;
    bool input_ready;
};
