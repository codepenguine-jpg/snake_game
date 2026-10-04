#include <ncurses.h>
#include <ctime>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

WINDOW *parent, *child;
constexpr int WINDOW_HEIGHT = 20, WINDOW_WIDTH = 60;
bool game_over;
int snake_headY, snake_headX, fruit_y, fruit_x;
enum Direction
{
    STOP,
    UP,
    DOWN,
    LEFT,
    RIGHT
};
Direction dir;

bool ate_fruit;
int tail_length, score;
int snake_tailY[WINDOW_HEIGHT * WINDOW_WIDTH] = {0}, snake_tailX[WINDOW_HEIGHT * WINDOW_WIDTH] = {0};
int old_headY, old_headX;

void game_terminate(void);
void game_paused(void);

void game_setup()
{
    if (parent == nullptr)
    {
        parent = newwin(WINDOW_HEIGHT + 6, WINDOW_WIDTH + 2, 0, 0);
        child = derwin(parent, WINDOW_HEIGHT, WINDOW_WIDTH, 3, 1);
    }
    game_over = false;

    snake_headY = WINDOW_HEIGHT / 2, snake_headX = WINDOW_WIDTH / 2;

    do
    {
        fruit_y = rand() % (WINDOW_HEIGHT - 2) + 1;
        fruit_x = rand() % (WINDOW_WIDTH - 2) + 1;
    } while (fruit_y == snake_headY && fruit_x == snake_headX);

    ate_fruit = false;
    tail_length = 0, score = 0;
    dir = STOP;
    old_headY = 0, old_headX = 0;
}

void game_draw(void)
{
    mvwaddch(child, snake_headY, snake_headX, 'O');
    mvwaddch(child, fruit_y, fruit_x, 'F');
    for (int i = 0; i < tail_length; i++)
    {
        mvwaddch(child, snake_tailY[i], snake_tailX[i], 'o');
    }

    mvwhline(parent, WINDOW_HEIGHT + 3, 1, 0, WINDOW_WIDTH - 1);
    mvwprintw(parent, WINDOW_HEIGHT + 4, 1, "Score: %d", score);
    wrefresh(parent);
    wrefresh(child);
}

void game_input(void)
{
    int key = wgetch(child);
    switch (key)
    {
    case 'W':
    case 'w':
    case KEY_UP:
        if (dir == DOWN)
            break;
        dir = UP;
        break;
    case 'S':
    case 's':
    case KEY_DOWN:
        if (dir == UP)
            break;
        dir = DOWN;
        break;
    case 'A':
    case 'a':
    case KEY_LEFT:
        if (dir == RIGHT)
            break;
        dir = LEFT;
        break;
    case 'D':
    case 'd':
    case KEY_RIGHT:
        if (dir == LEFT)
            break;
        dir = RIGHT;
        break;
    case 'q':
    case 27:
        game_over = true;
        break;
    case ' ':
        const char *str = "PAUSED!!!. Press SPACE/ENTER to play again.";
        mvwaddstr(child, (WINDOW_HEIGHT / 2), (WINDOW_WIDTH - strlen(str)) / 2, str);
        wrefresh(child);
        while (true)
        {
            int temp_key = wgetch(child);
            if (temp_key == ' ' || temp_key == '\n')
                break;
            else if (temp_key == 'q' || temp_key == 27)
            {
                game_over = true;
                break;
            }
        }
        break;
    }
}
/*
                                   ooo (x', y')
                                     o (x,y)
                                     o (new_x,new_y)
                                     O (new_x1, new_y1)
*/

void game_logic()
{
    old_headY = snake_headY, old_headX = snake_headX;

    switch (dir)
    {
    case STOP:
        break;
    case UP:
        snake_headY--;
        if (snake_headY <= 0)
        {
            game_terminate();
            return;
        }
        break;
    case DOWN:
        snake_headY++;
        if (snake_headY >= WINDOW_HEIGHT - 1)
        {
            game_terminate();
            return;
        }
        break;
    case LEFT:
        snake_headX--;
        if (snake_headX <= 0)
        {
            game_terminate();
            return;
        }
        break;
    case RIGHT:
        snake_headX++;
        if (snake_headX >= WINDOW_WIDTH - 1)
        {
            game_terminate();
            return;
        }
        break;
    }

    ate_fruit = snake_headY == fruit_y && snake_headX == fruit_x;

    if (ate_fruit)
    {
        score++;
        if (tail_length < WINDOW_HEIGHT * WINDOW_WIDTH - 1)
            tail_length++;
        do
        {
            fruit_y = rand() % (WINDOW_HEIGHT - 2) + 1;
            fruit_x = rand() % (WINDOW_WIDTH - 2) + 1;
        } while (snake_headY == fruit_y && snake_headX == fruit_x);
        ate_fruit = false;
    }

    for (int i = tail_length - 1; i >= 0; i--)
    {
        if (i == 0)
        {
            snake_tailY[i] = old_headY;
            snake_tailX[i] = old_headX;
        }
        else
        {
            snake_tailY[i] = snake_tailY[i - 1];
            snake_tailX[i] = snake_tailX[i - 1];
        }
    }

    // Collission detection and handling
    for (int i = 0; i < tail_length; i++)
    {
        if (snake_headX == snake_tailX[i] && snake_headY == snake_tailY[i])
        {
            game_terminate();
            return;
        }
    }
}

void game_terminate()
{
    game_over = true;
    const char *msg = "GAME OVER! Press SPACE/ENTER to restart";
    const char *quit_msg = "Press q or ESC to quit";
    mvwprintw(child, WINDOW_HEIGHT / 2 - 1, (WINDOW_WIDTH - strlen(msg)) / 2, "%s", msg);
    mvwprintw(child, WINDOW_HEIGHT / 2, (WINDOW_WIDTH - strlen(quit_msg)) / 2, "%s", quit_msg);
    wrefresh(child);

    wtimeout(child, -1);
    while (true)
    {
        int key = wgetch(child);
        if (key == '\n' || key == ' ')
        {
            game_setup();
            game_over = false;
            wtimeout(child, 100);
            return;
        }
        if (key == 'q' || key == 27)
            return;
    }
}

int main()
{
    initscr();
    noecho();
    cbreak();
    clear();
    curs_set(0);

    srand(static_cast<unsigned int>(time(nullptr)));
    game_setup();

    const char *welcome_msg = "WELCOME TO SNAKE WORLD";

    werase(parent);
    box(parent, 0, 0);
    mvwprintw(parent, 1, (WINDOW_WIDTH - strlen(welcome_msg)) / 2, welcome_msg);
    mvwhline(parent, 2, 1, 0, WINDOW_WIDTH - 1);
    wrefresh(parent);

    keypad(child, true);
    wtimeout(child, 100);

    while (!game_over)
    {
        werase(child);
        game_draw();
        game_input();
        if (game_over)
            break;
        game_logic();
        usleep(100000);
    }

    delwin(child);
    delwin(parent);
    endwin();
    return 0;
}
