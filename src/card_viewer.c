#include <stdio.h>
#include <unistd.h>

#include "access.h"
#include "cards.h"
#include "card_artwork.h"

int main(void)
{
    CardDatabase db;

    clear_screen();
    hide_cursor();

    if (!load_cards(&db, "src/card_info.json"))
    {
        show_cursor();
        reset_color();

        printf("Error: could not load \"card_info.json\".\n");

        return 1;
    }

    draw_all_cards(&db);

    gotoxy(1, terminal_height);

    reset_color();
    show_cursor();

    print_statistics(&db);

    return 0;
}