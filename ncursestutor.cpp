#include <cstdlib>
#include <ncurses.h>
#include <ctime>
#include <unistd.h>

int height = 10, width = 30, start_y = 5, start_x = 1;

int main() {
    /*
     * initscr() -> create new terminal
     * endwin()  -> delete terminal
     * printw()  -> print string
     * mvprintw() -> move to a position and print string
     * refresh() -> take value from output buffer and print it on screen
     *
     * newwin() -> WINDOW* -> create new WINDOW
     * delwin() -> delete WINDOW
     * wprintw() -> print string to window
     * mvwprintw()/mvwaddstr() -> move to a position in window and print string ( parameter -> string )
     * mvwaddch() -> print character in window
     * wrefresh() -> print value from ouput buffer to  ouput window
     * box() -> show border
     *
     * rand () -> srand(time)
     * noecho() -> does not show input in screen
     * cbreak() -> prevents storing buffer
     */
    

    srand(static_cast<unsigned int>(time(nullptr)));
    initscr();
    cbreak();
    noecho();
    printw("hello");
    refresh();
    // sleep / usleep -> unistd
    
    // create new window
    WINDOW* mywin = newwin(height, width, start_y, start_x);
    wprintw(mywin, "World");
    wrefresh(mywin);
    box(mywin, 'y', 'x');
    wrefresh(mywin);
    int a = 10;
    mvwprintw(mywin, 0, 7, "my name is saurabh = %d", a);
    wrefresh(mywin);
    mvwaddch(mywin, 2, 15, 'O');
    wrefresh(mywin);

    /*
    while (a-- > 0) {
        int random_value = rand();
        wprintw(mywin, "\nrandom value = %d\n", random_value);
        wrefresh(mywin);
    }
    */

    wgetch(mywin);
    sleep(5);

    delwin(mywin);
   getch();

    endwin();

    return 0;
}
