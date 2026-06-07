#include <bitset>
#include <mutex>
#include "ncurses.h"

#include "types.h"
#include "iodevice.h"
#include "shifter.h"

// A keyboard input device with a 4003 shifter, which mirrors the operation of
// the Busicom keyboard.

// To simulate how the Busicom keyboard works, input from the system
// (port_output) sets the keyboard column to read using a 10-bit shifter.
// Use "get_shifter" to access the shifter and connect it like all other devices
// Two ports are used: port 0 is the input and port 1 is the clock.
// See below for a further description of how we simulate shifter input.

// The current keyboard only uses three columns:
// 1  2  3
// 4  5  6
// 7  8  9
// Sp 0  Rt   (Sp = Space, Rt = return)
// Columns 3-9 are ignored.
class Keyboard : public iodevice, public testdevice
{
    public:
    Keyboard() :input_row(-1), input_col(-1)
    {
        shifter = std::make_shared<Shifter4003>();
    }

    // The Busicom keyboard reports if the target key has been pressed as
    // indicated by the shifter (column) and port id (row). We simply store
    // the most recent key pressed and clear it (set to -1) when read.
    // TODO: The Busicom supports pressing multiple keys at a time. We need to
    // revisit how to support that after we get the rest of the Busicom working.
    Bit port_input(int port_id)
    {
        assert((port_id >= 0) && (port_id <= 3));

        std::lock_guard<std::mutex> l(kb_buffer_mutex);

        if (!key_was_pressed()) return 0;

        // Note that 0 indicates the active column (not 1).
        if (!shifter->is_set(input_col) && input_row == port_id) {
            clear_pressed_key();
            return 1;
        }
        return 0;
    }

    // No output capability
    void port_output(int port_id, Bit val) {}

    // Indicates whether there are nibbles to be read.
    // (User must keep track of whether nibble is high or low.)
    Bit test() {
        std::lock_guard<std::mutex> l(kb_buffer_mutex);
        return key_was_pressed();
    }

    // The attached shifter functions like every other io device, so give the
    // user full access.
    std::shared_ptr<Shifter4003> get_shifter() const {
        return shifter;
    }

    void run()
    {
        // NCurses basic configuration
        keypad(stdscr, TRUE);
        mmask_t ignore;
        mousemask(BUTTON1_CLICKED, &ignore);

        init_keyboard();
        refresh();
        while(true) read_key(getch());
    }

    private:

    std::shared_ptr<Shifter4003> shifter;

    // Currently we only support 0-9, space, and return
    static const int NUM_ROWS = 4;
    static const int NUM_COLS = 3;

    int input_row;
    int input_col;
    int row_pos[NUM_ROWS];
    int col_pos[NUM_COLS];
    std::mutex kb_buffer_mutex;

    // Compute key positions and print the keyboard
    void init_keyboard()
    {
        const char key_matrix[NUM_ROWS][NUM_COLS] =
        {
            {'1', '2', '3'},
            {'4', '5', '6'},
            {'7', '8', '9'},
            {' ', '0', 'R'},
        };

        const int start_row = getmaxy(stdscr) * 2 / 3;
        const int start_col = 10;
        const int row_spaces = 3;
        const int col_spaces = 2;

        for (int i=0; i<NUM_ROWS; i++)
        {
            row_pos[i] = start_row + i*row_spaces;
            for (int j=0; j<NUM_COLS; j++)
            {
                col_pos[j] = start_col + j*col_spaces;
                mvprintw(row_pos[i], col_pos[j], "%c", key_matrix[i][j]);
            }
        }
    }

    // Convenience functions
    bool key_was_pressed() const
    {
        return (input_row > -1) && (input_col > -1);
    }

    void clear_pressed_key()
    {
        input_row = -1;
        input_col = -1;
    }

    void read_key(int input_event)
    {
        std::lock_guard<std::mutex> l(kb_buffer_mutex);

        // For now, we only recognize clicks of button 1
        MEVENT event;
        if (input_event == KEY_MOUSE && getmouse(&event) == OK && event.bstate & BUTTON1_CLICKED)
        {
            set_clicked_key(event.x, event.y);
        }
    }


    // Set the clicked row and column. Set to -1 if click is not on a
    // specific row or column.
    void set_clicked_key(int xmouse, int ymouse)
    {
        // Do not overwrite any current, stored key press
        if (key_was_pressed()) return;

        input_row = -1;
        for (int i=0; i<NUM_ROWS; i++)
        {
            if (ymouse == row_pos[i])
            {
               input_row = i;
               break;
            }
        }

        input_col = -1;
        for (int i=0; i<NUM_COLS; i++)
        {
            if (xmouse == col_pos[i])
            {
               input_col = i;
               break;
            }
        }
    }
};
