#include <string>
#include "ncurses.h"

const int log_win_left_margin = 80;
const int log_win_height      = 40;
WINDOW* log_window = nullptr;

void send_to_log(std::string s)
{
    if (log_window == nullptr)
    {
        log_window = newwin(log_win_height, log_win_left_margin, 0, log_win_left_margin);
        scrollok(log_window, TRUE);
        wrefresh(log_window);
    }

    wprintw(log_window, s.c_str());
    wprintw(log_window, "\n");
    wrefresh(log_window);
}
