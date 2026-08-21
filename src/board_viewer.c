#include <stdio.h>

#include "access.h"
#include "cards.h"
#include "card_artwork.h"
#include "board_art.h"

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

    draw_board();

    gotoxy(1, terminal_height);

    getchar();

    for (int j = 0; j < 2; j++)
       for (int i = 0; i < 5; i++)
            draw_card(board_slots[j][i].x, board_slots[j][i].y, &(db.cards[i]));

    draw_card(environment_slot.x, environment_slot.y, &(db.cards[20]));

    for (int i = 0; i < 4; i++)
        draw_card(hand_slots[i].x, hand_slots[i].y, &(db.cards[10 + i]));

    for (int i = 0; i < 3; i++)
        draw_card(stack_slots[i].x, stack_slots[i].y, &(db.cards[15 + i]));

    show_cursor();
    reset_color();

    return 0;
}