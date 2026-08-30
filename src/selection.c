#define _DEFAULT_SOURCE

#include "selection.h"

#include <stdio.h>
#include <unistd.h>
#include <termios.h>

#include "access.h"
#include "card_artwork.h"

/*=========================================================
    Terminal Input
=========================================================*/

static int read_key(void)
{
    struct termios old_termios;
    struct termios new_termios;

    int c;

    tcgetattr(STDIN_FILENO, &old_termios);

    new_termios = old_termios;

    new_termios.c_lflag &=
        ~(ICANON | ECHO);

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &new_termios);

    c = getchar();

    /*
        Arrow keys begin with ESC.
    */
    if (c == 27)
    {
        int c2 = getchar();

        if (c2 == '[')
        {
            int c3 = getchar();

            switch (c3)
            {
            case 'A':
                c = 'w';
                break;

            case 'B':
                c = 's';
                break;

            case 'C':
                c = 'd';
                break;

            case 'D':
                c = 'a';
                break;

            default:
                c = 27;
                break;
            }
        }
    }

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &old_termios);

    return c;
}

/*=========================================================
    Selection
=========================================================*/

int select_target(
    const SelectionOption *options,
    int option_count)
{
    int current = 0;
    int key;

    if (options == NULL && option_count > 0)
        return SELECT_CANCEL;

    /*
        Initial hover.
    */
    if (option_count > 0)
    {
        draw_outline(
            options[current].position.x,
            options[current].position.y,
            HOVER_COLOR);
    }

    while (1)
    {
        key = read_key();

        /*
            ------------------------------------------------
            Pass phase
            ------------------------------------------------
        */

        if (key == 'p' || key == 'P')
        {
            if (option_count > 0)
            {
                draw_outline(
                    options[current].position.x,
                    options[current].position.y,
                    options[current].normal_color);
            }

            return SELECT_PASS;
        }

        /*
            ------------------------------------------------
            Cancel
            ------------------------------------------------
        */

        if (key == 27)
        {
            if (option_count > 0)
            {
                draw_outline(
                    options[current].position.x,
                    options[current].position.y,
                    options[current].normal_color);
            }

            return SELECT_CANCEL;
        }

        /*
            ------------------------------------------------
            No selectable options
            ------------------------------------------------

            There is nothing else to do. Keep waiting for
            an interrupt such as P.
            ------------------------------------------------
        */

        if (option_count == 0)
            continue;

        /*
            ------------------------------------------------
            Normal selection
            ------------------------------------------------
        */

        if (key == 'a' || key == 'A')
        {
            if (current > 0)
            {
                draw_outline(
                    options[current].position.x,
                    options[current].position.y,
                    options[current].normal_color);

                current--;

                draw_outline(
                    options[current].position.x,
                    options[current].position.y,
                    HOVER_COLOR);
            }
        }

        else if (key == 'd' || key == 'D')
        {
            if (current < option_count - 1)
            {
                draw_outline(
                    options[current].position.x,
                    options[current].position.y,
                    options[current].normal_color);

                current++;

                draw_outline(
                    options[current].position.x,
                    options[current].position.y,
                    HOVER_COLOR);
            }
        }

        else if (key == 'w' || key == 'W')
        {
            if (current > 0)
            {
                draw_outline(
                    options[current].position.x,
                    options[current].position.y,
                    options[current].normal_color);

                current--;

                draw_outline(
                    options[current].position.x,
                    options[current].position.y,
                    HOVER_COLOR);
            }
        }

        else if (key == 's' || key == 'S')
        {
            if (current < option_count - 1)
            {
                draw_outline(
                    options[current].position.x,
                    options[current].position.y,
                    options[current].normal_color);

                current++;

                draw_outline(
                    options[current].position.x,
                    options[current].position.y,
                    HOVER_COLOR);
            }
        }

        else if (key == '\n' ||
                 key == '\r')
        {
            draw_outline(
                options[current].position.x,
                options[current].position.y,
                SELECTION_COLOR);

            usleep(500000);

            draw_outline(
                options[current].position.x,
                options[current].position.y,
                options[current].normal_color);

            return current;
        }
    }
}