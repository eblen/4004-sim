#include <mutex>
#include "ncurses.h"

#include "types.h"
#include "iodevice.h"

// A simple input device for inputting hexadecimal digits
class Hexpad: public iodevice, public testdevice
{
    public:
    Hexpad() :user_input(0), input_ready(false) {}

    // Input one nibble at a time upon request.
    Nibble port_input()
    {
        std::lock_guard<std::mutex> l(kb_buffer_mutex);
        input_ready = false;
        return user_input;
    }

    // No output capability
    void port_output(Nibble val) {}

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
