#include <bitset>
#include <mutex>
#include "ncurses.h"

#include "types.h"
#include "iodevice.h"
#include "shifter.h"

// A simple input device for debugging

// To simulate how the Busicom keyboard works, input from the system
// (port_output) sets the keyboard column to read using a 10-bit shifter.
// Two ports are used: port 0 is the input and port 1 is the clock.
// See below for a further description of how we simulate shifter input.

// For output to the system (port_input), we currently only use three columns:
// 1  2  3
// 4  5  6
// 7  8  9
// Sp 0  Rt   (Sp = Space, Rt = return)
// Columns 3-9 are ignored.
class Keyboard : public iodevice, public testdevice
{
    public:
    Keyboard() :maxx(0), maxy(0), buffer{0,0}, bit_read_status(0), input_column(0),
                num_input_nibbles(0)
    {
        shifter = std::make_shared<Shifter4003>();
    }

    // The Busicom keyboard reports what key is pressed (assuming only one)
    // for the column indicated by the shifter. To avoid timing and interface
    // issues, we simply store the most recent key press and report it if it
    // is on the inidicated column. We also assume only one key has been
    // pressed at a time.
    // TODO: The Busicom supports pressing multiple keys at a time. We need to
    // revisit how to support that after we get the rest of the Busicom working.
    Bit port_input(int port_id)
    {
        assert((port_id >= 0) && (port_id <= 3));
        // Avoid by only calling when "test" returns true
        assert(num_input_nibbles > 0);

        std::lock_guard<std::mutex> l(kb_buffer_mutex);

        // Indicates bit was read twice for the same nibble
        if (get_bit(bit_read_status, port_id) == 1) assert(false);

        bit_read_status = set_bit(bit_read_status, port_id, 1);

	// Return bit in buffer only if shifter points to the correct column
	// Note that 0 indicates the active column (not 1).
	Bit ret_val = shifter->is_set(input_column) ? 0 : get_bit(buffer[0], port_id);

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

    // The attached shifter functions like every other io device, so give the
    // user full access.
    std::shared_ptr<Shifter4003> get_shifter() {
	return shifter;
    }

    // User can input a single char at a time, which is broken into 2 nibbles.
    void read_char(int input_event)
    {
        std::lock_guard<std::mutex> l(kb_buffer_mutex);
        // A primitive device that does not store more than one byte at a time for output.
        if (num_input_nibbles > 0) return;

        // For now, we only recognize clicks of button 1
        MEVENT event;
        if (input_event == KEY_MOUSE && getmouse(&event) == OK && event.bstate & BUTTON1_CLICKED)
        {
            int input_value = get_clicked_key(event.x, event.y);
            if (input_value == -1) return;

            // Since we now input single digits, just store the value instead
            // of an ASCII code.
            buffer[0] = 0;
            buffer[1] = input_value;
            num_input_nibbles = 2;
            input_column = key_columns[input_value];
        }
    }

    void run()
    {
        // NCurses basic configuration
        getmaxyx(stdscr, maxy, maxx);
        keypad(stdscr, TRUE);
        mmask_t ignore;
        mousemask(BUTTON1_CLICKED, &ignore);

        init_keyboard();
        refresh();
        while(true) read_char(getch());
    }

    private:

    std::shared_ptr<Shifter4003> shifter;

    // Currently we only support 0-9, space, and return
    #define NUM_KEYS 12
    const int SPACE_KEY = 10;
    const int RETURN_KEY = 11;

    // Table to support using the shifter
    const int key_columns[12] = {1,0,1,2,0,1,2,0,1,2,0,2};

    int input_column;
    int key_to_pixel[NUM_KEYS];
    int maxx, maxy;
    Nibble buffer[2];
    Nibble bit_read_status;
    Crumb num_input_nibbles;
    std::mutex kb_buffer_mutex;

    // Compute key positions and print the keyboard
    void init_keyboard()
    {
        // Compute pixel positions for each key
        const int start_row = maxy * 2 / 3;
        const int start_col = 10;
        const int row_spaces = 3;
        const int col_spaces = 2;

        // First row
        int start_pixel = start_row*maxx;
        key_to_pixel[1] = start_pixel + start_col;
        key_to_pixel[2] = key_to_pixel[1] + row_spaces;
        key_to_pixel[3] = key_to_pixel[2] + row_spaces;

        // Second row
        start_pixel += col_spaces*maxx;
        key_to_pixel[4] = start_pixel + start_col;
        key_to_pixel[5] = key_to_pixel[4] + row_spaces;
        key_to_pixel[6] = key_to_pixel[5] + row_spaces;

        // Third row
        start_pixel += col_spaces*maxx;
        key_to_pixel[7] = start_pixel + start_col;
        key_to_pixel[8] = key_to_pixel[7] + row_spaces;
        key_to_pixel[9] = key_to_pixel[8] + row_spaces;

        // Space, zero, and return
        start_pixel += col_spaces*maxx;
        key_to_pixel[SPACE_KEY]  = start_pixel + start_col;
        key_to_pixel[0]          = key_to_pixel[SPACE_KEY] + row_spaces;
        key_to_pixel[RETURN_KEY] = key_to_pixel[0] + row_spaces;

        // Print the keyboard
        // Note that row spacing must match "row_spaces" defined above
        // TODO: Consider how to use "row_spaces" without making the code
        // overly complex.
        mvprintw(start_row,                start_col, "1  2  3");
        mvprintw(start_row +   col_spaces, start_col, "4  5  6");
        mvprintw(start_row + 2*col_spaces, start_col, "7  8  9");
        mvprintw(start_row + 3*col_spaces, start_col, "   0  $");
    }

    // Return key clicked for the given mouse coordinates
    // Return -1 if no match found
    int get_clicked_key(int xmouse, int ymouse)
    {
        int clicked_pixel = ymouse*maxx + xmouse;
        for (int i=0; i<NUM_KEYS; i++)
        {
            if (key_to_pixel[i] == clicked_pixel) return i;
        }
        return -1;
    }
};
