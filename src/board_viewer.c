#include <stdio.h>

#include "access.h"
#include "board_art.h"

int main(void)
{
    clear_screen();

    hide_cursor();

    draw_board();

    getchar();

    show_cursor();
    reset_color();

    return 0;
}
