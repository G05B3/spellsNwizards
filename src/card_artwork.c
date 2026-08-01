#include "card_artwork.h"

#include <stdio.h>
#include <string.h>

/*=========================================================
    Primitive Drawing
=========================================================*/

void draw_outline(
    int x,
    int y,
    Color color)
{
    int i, j;

    set_color(color);

    gotoxy(x, y);

    printf(TL);

    for (i = 0; i < CLENGTH; i++)
        printf(HZ);

    printf(TR);

    for (j = 0; j < CHEIGHT; j++)
    {
        gotoxy(x, y + j + 1);

        if (j != 1)
        {
            printf(VL);

            for (i = 0; i < CLENGTH; i++)
                printf(" ");

            printf(VL);
        }
        else
        {
            printf(D1);

            for (i = 0; i < CLENGTH; i++)
                printf(HZ);

            printf(D2);
        }
    }

    gotoxy(x, y + CHEIGHT + 1);

    printf(BL);

    for (i = 0; i < CLENGTH; i++)
        printf(HZ);

    printf(BR);

    reset_color();
}


/*-------------------------------------------------------*/

void draw_card(
    int x,
    int y,
    const Card *card)
{
    int len;

    draw_outline(x, y, WHITE);

    /*
        Card name
    */

    len = strlen(card->name);

    draw_text(
        x + CLENGTH / 2 - len / 2 + 1,
        y + 1,
        WHITE,
        "%s",
        card->name);

    /*
        Cost
    */

    gotoxy(x + CLENGTH + 2, y);

    printf(TL HZ HZ TR);

    gotoxy(x + CLENGTH + 2, y + 1);

    printf(VL "  " VL);

    gotoxy(x + CLENGTH + 2, y + 2);

    printf(BL HZ HZ BR);

    draw_text(
        x + CLENGTH + 3,
        y + 1,
        CYAN,
        "%2d",
        0);

    /*
        Creature stats

        Replace condition later by

            card->isCreature
    */

    if (1 /* card->isCreature == TRUE */)
    {
        /*
            Attack
        */

        gotoxy(x + CLENGTH + 2,
               y + CHEIGHT - 4);

        printf(TL HZ HZ TR);

        gotoxy(x + CLENGTH + 2,
               y + CHEIGHT - 3);

        printf(VL "  " VL);

        gotoxy(x + CLENGTH + 2,
               y + CHEIGHT - 2);

        printf(BL HZ HZ BR);

        draw_text(
            x + CLENGTH + 3,
            y + CHEIGHT - 3,
            RED,
            "%2d",
            0);

        /*
            HP
        */

        gotoxy(x + CLENGTH + 2,
               y + CHEIGHT - 1);

        printf(TL HZ HZ TR);

        gotoxy(x + CLENGTH + 2,
               y + CHEIGHT);

        printf(VL "  " VL);

        gotoxy(x + CLENGTH + 2,
               y + CHEIGHT + 1);

        printf(BL HZ HZ BR);

        draw_text(
            x + CLENGTH + 3,
            y + CHEIGHT,
            GREEN,
            "%2d",
            0);
    }

    /*
        Artwork
    */

    draw_image(x, y, card);
}


/*=========================================================
    Default Artwork
=========================================================*/

void draw_unknown(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(
        x + 5,
        y + 5,
        p[0],
        "Unknown");

    draw_line(
        x + 7,
        y + 6,
        p[1],
        "Art");
}

/*=========================================================
    Green Dragon
=========================================================*/

void draw_dragon(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 2,y + 4,p[0],"     ((     ))");
    draw_line(x + 2,y + 5,p[0]," ===  \\\\_v_//  ===");
    draw_line(x + 2,y + 6,p[0],"   ====)_^_(====");
    draw3(x + 2,y + 7,p[0], " =====/ ",p[1], "O O",p[0], " \\=====");
    draw_line(x + 2,y + 8,p[0],"   = | /_ _\\ | =");
    draw_line(x + 2,y + 9,p[0],"  =   \\/_ _\\/   =");
    draw_line(x + 2,y + 10,p[0],"       \\_ _/");
    draw_line(x + 2,y + 11,p[0],"       (o_o)");
    draw_line(x + 2,y + 12,p[0],"        VwV");
}

/*=========================================================
    Warrior
=========================================================*/

void draw_warrior(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 2, y + 4, p[0], "       ,");
    draw_line(x + 2, y + 5, p[0], "   ,   |\\ ,__");
    draw_line(x + 2, y + 6, p[0], "   |\\   \\/   `.");
    draw_line(x + 2, y + 7, p[0], "   \\ `-.:.     `\\");
    draw_line(x + 2, y + 8, p[0], "    `-.__ `\\=====|");
    draw(x + 2, y + 9, p[0], "       /=`'", p[1], "/   ^_\\");
    draw_line(x + 2, y + 10, p[1], "     .'   /\\   .=)");
    draw_line(x + 2, y + 11, p[1], "  .-'  .'|  '-(/_|");
    draw_line(x + 2, y + 12, p[1], ".'  __(  \\  .'`");
}

/*=========================================================
    Mage
=========================================================*/

void draw_mage(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 2, y + 4,  p[0], "              _,-'|");
    draw_line(x + 2, y + 5,  p[0], "           ,-'._  |");
    draw4(x + 2, y + 6, p[1], " .||,      ", p[0], "|", p[2], "####", p[0], "\\ |");
    draw4(x + 2, y + 7, p[1], "\\.`',/     ", p[0], "\\", p[2], "####", p[0], "| |");
    draw4(x + 2, y + 8, p[1], "= ,. =      ", p[0], "|", p[2], "###", p[0], "| |");
    draw4(x + 2, y + 9, p[1], "/ || \\    ", p[0], ",-'\\", p[2], "#", p[0], "/,'`");
    draw_line(x + 2, y + 10,  p[1], "  ||     ");
    draw_line(x + 11, y + 10, p[0], ",'   `,,. ");
    draw_line(x + 2, y + 11, p[1], "  ,|");
    draw_line(x + 6, y + 11, p[0], "____,' , ,;' \\|");
    draw3(x + 2, y + 12, p[0], " (3", p[1], "|", p[0], "\\    _/|/'   _|");
}


/*=========================================================
    Goblin
=========================================================*/

void draw_goblin(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 4,  p[0], "       .----.");
    draw_line(x + 1, y + 5,  p[0], " |\\   /      \\   /|");
    draw_line(x + 1, y + 6,  p[0], " | \\ / =.  .= \\ / |");
    draw_line(x + 1, y + 7,  p[0], " \\( \\   ");
    draw3(x + 9, y + 7, p[1], "o", p[0], "\\/", p[1], "o");
    draw_line(x + 13, y + 7, p[0],"   / )/");
    draw_line(x + 1, y + 8,  p[0], "  \\_, '-/  \\-' ,_/");
    draw_line(x + 1, y + 9,  p[0], "    /   \\__/   \\");
    draw_line(x + 1, y + 10,  p[0], "    \\ \\__/\\__/ /");
    draw_line(x + 1, y + 11, p[0], "  ___\\ \\|--|/ /___");
    draw_line(x + 1, y + 12, p[0], "/`    \\      /    `\\");
}

/*=========================================================
    Priest
=========================================================*/

void draw_priest(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 5, p[0], "              _,._");
    draw(x + 1, y + 6,p[1], "  .||,",p[0], "       /_ _\\\\");
    draw(x + 1, y + 7,p[1], " \\.`',/ ",p[0], "     |'L'| |");
    draw(x + 1, y + 8,p[1], " = ,. =",p[0], "      | -,| L");
    draw(x + 1, y + 9,p[1], " / || \\",p[0], "    ,-'\\\"/,'`");
    draw(x + 1, y + 10,p[1], "   ||",p[0], "     ,'   `,,. ");
    draw(x + 1, y + 11,p[1], "   ,|",p[0], "____,' , ,;' \\|");
    draw3(x + 1, y + 12, p[0], "  (3", p[1], "|", p[0], "\\    _/|/'   _|");
}


/*=========================================================
    Dispatcher
=========================================================*/

void draw_image(
    int x,
    int y,
    const Card *card)
{
    switch (card->image_id)
    {
        case 0:
            draw_warrior(x, y, card);
            break;

        case 1:
            draw_mage(x, y, card);
            break;

        case 2:
            draw_priest(x, y, card);
            break;

        // Armored Dragon, Flaming Dragon, Wise Dragon, Ancient Dragon, Bone Dragon
        case 3:
            draw_dragon(x, y, card);
            break;

        // Goblin, Hobgoblin
        case 4:
            draw_goblin(x, y, card);
            break;

        default:
            draw_unknown(x, y, card);
            break;
    }
}


/*=========================================================
    Layout
=========================================================*/

void draw_all_cards(
    const CardDatabase *db)
{
    int per_row;
    int row;
    int col;
    int i;

    int x;
    int y;

    get_terminal_size();

    per_row = cards_per_row();

    for (i = 0; i < db->card_count; i++)
    {
        row = i / per_row;
        col = i % per_row;

        /*
            3 spaces left margin.
        */

        x = 3 + col * (CLENGTH + CARD_SPACING);

        /*
            Two empty lines between rows.
        */

        y = 1 + row * (CHEIGHT + 3);

        draw_card(
            x,
            y,
            &db->cards[i]);
    }

    /*
        Leave the cursor below all cards.
    */

    gotoxy(
        1,
        2 + ((db->card_count + per_row - 1) / per_row) * (CHEIGHT + 3));

    reset_color();
    show_cursor();
}