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

            gotoxy(x + CLENGTH + 1, y + j + 1);
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
    fflush(stdout);
}

/*-------------------------------------------------------*/

void draw_card(
    int x,
    int y,
    const Card *card)
{
    int len;

    Color outline_color;

    if (card->card_type == CARD_ENVIRONMENT || card->card_type == CARD_NONE)
        outline_color = card->palette[0];
    else
        outline_color = CARD_TYPE_COLORS[card->card_type];

    draw_outline(x, y, outline_color);

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

    if (card->card_type != CARD_NONE)
    {
        set_color(outline_color);

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
            card->cost);

        reset_color();

        /*
            Creature stats

            Replace condition later by

                card->isCreature
        */

        if (card->card_type == CARD_CREATURE)
        {
            /*
                Attack
            */

            set_color(outline_color);

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
                ATK_TYPE_COLORS[card->atk_type],
                "%2d",
                card->atk);

            /*
                HP
            */

            set_color(outline_color);

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
                card->hp);
        }
    }

    /*
        Artwork
    */

    draw_image(x, y, card);
}

void delete_card(int x, int y)
{
    int i, j;

    for (i = 0; i < CHEIGHT + 2; i++)
    {
        gotoxy(x, y + i);
        for (j = 0; j < CLENGTH + 6; j++)
            printf(" ");
    }
}

/*=========================================================
    Empty
=========================================================*/

void draw_empty(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[0], "                    ");
    draw_line(x + 1, y + 4, p[0], "                    ");
    draw_line(x + 1, y + 5, p[0], "                    ");
    draw_line(x + 1, y + 6, p[0], "                    ");
    draw_line(x + 1, y + 7, p[0], "                    ");
    draw_line(x + 1, y + 8, p[0], "                    ");
    draw_line(x + 1, y + 9, p[0], "                    ");
    draw_line(x + 1, y + 10, p[1], "                    ");
    draw_line(x + 1, y + 11, p[1], "                    ");
    draw_line(x + 1, y + 12, p[1], "                    ");
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

    draw_line(x + 2, y + 4, p[0], "     ((     ))");
    draw_line(x + 2, y + 5, p[0], " ===  \\\\_v_//  ===");
    draw_line(x + 2, y + 6, p[0], "   ====)_^_(====");
    draw3(x + 2, y + 7, p[0], " =====/ ", p[1], "O O", p[0], " \\=====");
    draw_line(x + 2, y + 8, p[0], "   = | /_ _\\ | =");
    draw_line(x + 2, y + 9, p[0], "  =   \\/_ _\\/   =");
    draw_line(x + 2, y + 10, p[0], "       \\_ _/");
    draw_line(x + 2, y + 11, p[0], "       (o_o)");
    draw_line(x + 2, y + 12, p[0], "        VwV");
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

    bool skeleton_mage = (p[0] == PURPLE && p[1] == MAGENTA && p[2] == GRAY);

    draw_line(x + 2, y + 4, p[0], "              _,-'|");
    draw_line(x + 2, y + 5, p[0], "           ,-'._  |");
    if (skeleton_mage)
    {
        draw4(x + 2, y + 6, p[1], " .||,      ", p[0], "|", p[2], "()()", p[0], "\\ |");
        draw4(x + 2, y + 7, p[1], "\\.`',/     ", p[0], "\\", p[2], " 4A ", p[0], "| |");
        draw4(x + 2, y + 8, p[1], "= ,. =      ", p[0], "|", p[2], "nnm", p[0], "| |");
        draw4(x + 2, y + 9, p[1], "/ || \\    ", p[0], ",-'\\", p[2], "v", p[0], "/,'`");
    }
    else
    {
        draw4(x + 2, y + 6, p[1], " .||,      ", p[0], "|", p[2], "####", p[0], "\\ |");
        draw4(x + 2, y + 7, p[1], "\\.`',/     ", p[0], "\\", p[2], "####", p[0], "| |");
        draw4(x + 2, y + 8, p[1], "= ,. =      ", p[0], "|", p[2], "###", p[0], "| |");
        draw4(x + 2, y + 9, p[1], "/ || \\    ", p[0], ",-'\\", p[2], "#", p[0], "/,'`");
    }
    draw_line(x + 2, y + 10, p[1], "  ||     ");
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

    bool hobgoblin = (p[0] == ORANGE && p[1] == YELLOW && p[2] == WHITE);
    bool big_goblin = (p[0] == OLIVE && p[1] == YELLOW && p[2] == WHITE);

    if (hobgoblin)
    {
        draw_line(x + 1, y + 4, p[0], "     /(.----.)\\");
        draw_line(x + 1, y + 5, p[0], " |\\  \\/      \\/  _");
    }
    else
    {
        draw_line(x + 1, y + 4, p[0], "       .----.");
        draw_line(x + 1, y + 5, p[0], " |\\   /      \\   /|");
    }
    if (big_goblin)
    {
        draw_line(x + 1, y + 6, p[0], " | \\ / ^~vv~^ \\ / |");
    }
    else
    {
        draw_line(x + 1, y + 6, p[0], " | \\ / =.  .= \\ / |");
    }
    draw_line(x + 1, y + 7, p[0], " \\( \\   ");
    draw3(x + 9, y + 7, p[1], "o", p[0], "\\/", p[1], "o");
    draw_line(x + 13, y + 7, p[0], "   / )/");
    draw_line(x + 1, y + 8, p[0], "  \\_, '-/  \\-' ,_/");
    draw_line(x + 1, y + 9, p[0], "    /   \\__/   \\");
    draw_line(x + 1, y + 10, p[0], "    \\ \\__/\\__/ /");
    draw3(x + 1, y + 11, p[0], "  ___\\ ", p[2], "\\|--|/", p[0], " /___");
    draw_line(x + 1, y + 12, p[0], "/`    \\      /    `\\");
}

/*=========================================================
    Spear Goblin
=========================================================*/

void draw_spear_goblin(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[5], " .");
    draw(x + 1, y + 4, p[5], "/ \\      ", p[0], ".----.");
    draw4(x + 1, y + 5, p[5], "\\./", p[0], "|\\   /", p[3], "______", p[0], "\\   /");
    draw4(x + 1, y + 6, p[4], " | ", p[0], "| \\ ", p[3], "|________| ", p[0], "/ ");
    draw6(x + 1, y + 7, p[4], " | ", p[0], "\\( \\   ", p[1], "o", p[0], "\\/", p[1], "o   ", p[0], "/ )");
    draw(x + 1, y + 8, p[4], " |  ", p[0], "\\_, '-/  \\-' ,_/");
    draw(x + 1, y + 9, p[4], " |    ", p[0], "/   \\__/   \\");
    draw(x + 1, y + 10, p[4], " |    ", p[0], "\\ \\__/\\__/ /");
    draw4(x + 1, y + 11, p[4], " |  ", p[0], "___\\ ", p[2], "\\|--|/ ", p[0], "/__");
    draw(x + 1, y + 12, p[4], " | ", p[0], "/    \\      /    ");
}

/*=========================================================
    Goblin Chieftain
=========================================================*/

void draw_goblin_chief(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[3], "       /\\/\\/\\");
    draw_line(x + 1, y + 4, p[3], "      /||||||\\");
    draw3(x + 1, y + 5, p[0], " |\\  ", p[3], "/________\\  ", p[0], "/|");
    draw3(x + 1, y + 6, p[0], " | \\ / ~~  .", p[4], ") ", p[0], "\\ / |");
    draw5(x + 1, y + 7, p[0], " \\( \\   ", p[1], "o", p[0], "\\/", p[4], "x   ", p[0], "/ )/");
    draw3(x + 1, y + 8, p[0], "  \\_, '-/ ", p[4], "(", p[0], "\\-' ,_/");
    draw_line(x + 1, y + 9, p[0], "    /   \\__/   \\");
    draw_line(x + 1, y + 10, p[0], "    \\ ___/\\___ /");
    draw3(x + 1, y + 11, p[0], "  ___\\ ", p[2], "\\|  vv ", p[0], "/___");
    draw_line(x + 1, y + 12, p[0], " /    \\      /    \\");
}

/*=========================================================
    Mana Hoarder
=========================================================*/

void draw_mana_hoarder(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[4], "        *");
    draw_line(x + 1, y + 4, p[4], "        *x*");
    draw(x + 1, y + 5, p[4], "         **    ", p[5], "**");
    draw(x + 1, y + 6, p[2], "        _____ ", p[5], "*x*");
    draw4(x + 1, y + 7, p[2], "      / ", p[3], "v v v ", p[2], "\\ ", p[5], "*");
    draw3(x + 1, y + 8, p[0], "    )/", p[2], "\\_______/", p[0], "\\(_");
    draw_line(x + 1, y + 9, p[0], "  /    .-----.    \\");
    draw_line(x + 1, y + 10, p[0], " | |\\ /       \\ /| |");
    draw3(x + 1, y + 11, p[0], " | | \\\\ ", p[1], "o   o ", p[0], "// | |");
    draw_line(x + 1, y + 12, p[0], " |  \\( \\  ^  / )/  |");
}

/*=========================================================
    Goblin Shaman
=========================================================*/

void draw_goblin_shaman(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[4], " .-.        /\\");
    draw3(x + 1, y + 4, p[4], "(", p[5], "o o", p[4], ")      /  \\");
    draw3(x + 1, y + 5, p[4], " \\_/", p[0], "|\\  ", p[4], "\\_/____\\_/  ");
    draw(x + 1, y + 6, p[3], "  | ", p[0], "| \\ _/=.  .=\\_ /");
    draw6(x + 1, y + 7, p[3], "  | ", p[0], "\\( \\   ", p[1], "o", p[0], "\\/", p[1], "o   ", p[0], "/ ");
    draw(x + 1, y + 8, p[3], "  |  ", p[0], "\\_, '-/  \\-' ,_");
    draw(x + 1, y + 9, p[3], "  |\\   ", p[0], "/   \\__/   \\");
    draw(x + 1, y + 10, p[3], "  | \\ ", p[0], " \\ \\__/\\__/ /");
    draw4(x + 1, y + 11, p[3], "  |  ", p[0], "___\\ ", p[2], "\\|--|/ ", p[0], "/__");
    draw(x + 1, y + 12, p[3], "  | ", p[0], "/    \\      /  ");
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
    draw(x + 1, y + 6, p[1], "  .||,", p[0], "       /_ _\\\\");
    draw(x + 1, y + 7, p[1], " \\.`',/ ", p[0], "     |'L'| |");
    draw(x + 1, y + 8, p[1], " = ,. =", p[0], "      | -,| L");
    draw(x + 1, y + 9, p[1], " / || \\", p[0], "    ,-'\\\"/,'`");
    draw(x + 1, y + 10, p[1], "   ||", p[0], "     ,'   `,,. ");
    draw(x + 1, y + 11, p[1], "   ,|", p[0], "____,' , ,;' \\|");
    draw3(x + 1, y + 12, p[0], "  (3", p[1], "|", p[0], "\\    _/|/'   _|");
}

/*=========================================================
    Draw Horn
=========================================================*/

void draw_horn(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 5, y + 5, p[0], " ___");
    draw3(x + 5, y + 6, p[0], "(", p[1], "___", p[0], "(");
    draw3(x + 5, y + 7, p[0], " \\", p[1], "___", p[0], "\\");
    draw_line(x + 5, y + 8, p[0], "  \\   \\");
    draw3(x + 5, y + 9, p[0], "   \\", p[1], "  ", p[0], "|");
    draw3(x + 5, y + 10, p[0], "    \\", p[1], "_", p[0], "|");
    draw_line(x + 5, y + 11, p[0], "     \\|");
}

/*=========================================================
    Thief
=========================================================*/

void draw_thief(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 4, p[0], "          ______");
    draw_line(x + 1, y + 5, p[0], "        .'      `.");
    draw_line(x + 1, y + 6, p[0], "  /.   /          \\");
    draw_line(x + 1, y + 7, p[0], "  '.'.:            :");
    draw_line(x + 1, y + 8, p[0], "  _.:'|   ,--------|");
    draw3(x + 1, y + 9, p[0], "  '-.'|  | ", p[1], "v     v", p[0], " :");
    draw_line(x + 1, y + 10, p[0], "     :  \\_.---`**-.*");
    draw_line(x + 1, y + 11, p[0], "   __  \\           ;");
    draw_line(x + 1, y + 12, p[0], ".-'\\__  :         /");
}

/*=========================================================
    Skeleton
=========================================================*/

void draw_skeleton(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[0], "       __ _");
    draw_line(x + 1, y + 4, p[0], "     .'  Y '>,");
    draw_line(x + 1, y + 5, p[0], "    / _   _   \\");
    draw_line(x + 1, y + 6, p[0], "    )(_) (_)(|}");
    draw_line(x + 1, y + 7, p[0], "    {  4A   } /");
    draw_line(x + 1, y + 8, p[0], "     \\uLuJJ/\\l");
    draw_line(x + 1, y + 9, p[0], "     |3    p)/");
    draw_line(x + 1, y + 10, p[0], "     /nnm_n//");
    draw_line(x + 1, y + 11, p[0], "     \\_>-<_/");
}

/*=========================================================
    Question Mark (e.g., Amnesia)
=========================================================*/

void draw_question(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 6, y + 4, p[0], "  ______");
    draw_line(x + 6, y + 5, p[0], " |######|");
    draw_line(x + 6, y + 6, p[0], " |#|  |#|");
    draw_line(x + 6, y + 7, p[0], "    __|#|");
    draw_line(x + 6, y + 8, p[0], "   |####|");
    draw_line(x + 6, y + 9, p[0], "   |#|");
    draw_line(x + 6, y + 10, p[0], "    _");
    draw_line(x + 6, y + 11, p[0], "   |#|");
}

/*=========================================================
    Mist
=========================================================*/

void draw_mist(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[0], "   ###          ####");
    draw_line(x + 1, y + 4, p[0], "##########   #######");
    draw_line(x + 1, y + 5, p[0], "#########   ########");
    draw_line(x + 1, y + 6, p[0], " #######     ######");
    draw_line(x + 1, y + 7, p[0], "#########   ");
    draw_line(x + 1, y + 8, p[0], " ###########   ### ");
    draw_line(x + 1, y + 9, p[0], "   #######    ######");
    draw_line(x + 1, y + 10, p[0], "                ###");
    draw_line(x + 1, y + 11, p[0], "       #######");
}

/*=========================================================
    Soldier
=========================================================*/

void draw_soldier(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    bool dragonslayer_art = (p[0] == BLUE && p[1] == WHITE && p[2] == RED && p[3] == YELLOW);

    if (dragonslayer_art)
    {
        draw_line(x + 1, y + 4, p[0], "         ) ,^^. (");
        draw(x + 1, y + 5, p[2], "      .", p[0], " /(;\\  /:)\\");
        draw3(x + 1, y + 6, p[2], "     /.", p[1], "\\", p[0], "\\/  \\/  \\/");
        draw4(x + 1, y + 7, p[1], "     |.|", p[0], "<.<_\\/", p[3], "#", p[0], ">,>  ");
    }
    else
    {
        draw_line(x + 1, y + 4, p[0], "           ,'`.     ");
        draw(x + 1, y + 5, p[1], "      .", p[0], "   ;\\  /:    ");
        draw(x + 1, y + 6, p[1], "     /.\\", p[0], " /  \\/  \\   ");
        draw(x + 1, y + 7, p[1], "     |.|", p[0], "<.<_\\/_>,>  ");
    }
    draw(x + 1, y + 8, p[1], "     |.| ", p[0], "\\`.::,'/   ");
    draw(x + 1, y + 9, p[1], "     |.|", p[0], ",'.'||'/.  ");
    draw3(x + 1, y + 10, p[0], "  ,-'", p[1], "|.|", p[0], ".`.____,'`. ");
    draw3(x + 1, y + 11, p[0], ",' .`", p[1], "|.| ", p[0], "`.____,;/ \\");
    draw3(x + 1, y + 12, p[0], "'=-.`", p[1], "|.|", p[0], "\\ .   \\ |,':");
}

/*=========================================================
    Nature's Blessing
=========================================================*/

void draw_particles(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 4, p[0], "                 *  ");
    draw_line(x + 1, y + 5, p[0], "                x*  ");
    draw_line(x + 1, y + 6, p[0], "                *   ");
    draw_line(x + 1, y + 7, p[1], "  *                 ");
    draw_line(x + 1, y + 8, p[1], "  *x*               ");
    draw(x + 1, y + 9, p[1], "   **", p[2], "     **        ");
    draw_line(x + 1, y + 10, p[2], "         *x*        ");
    draw_line(x + 1, y + 11, p[2], "          *        ");
}

/*=========================================================
    Embers
=========================================================*/

void draw_embers(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 4, p[0], "                '   ");
    draw(x + 1, y + 5, p[1], "               ;", p[0], "(   ");
    draw3(x + 1, y + 6, p[0], "             (", p[2], " . ", p[1], ") ");
    draw_line(x + 1, y + 7, p[0], "   '                ");
    draw(x + 1, y + 8, p[1], "  ;", p[0], "(                ");
    draw3(x + 1, y + 9, p[1], "(", p[2], " . ", p[0], ")        ");
    draw_line(x + 1, y + 10, p[0], "        '           ");
    draw(x + 1, y + 11, p[1], "       ;", p[0], "(           ");
    draw_line(x + 1, y + 12, p[2], "       .            ");
}

/*=========================================================
    Knight
=========================================================*/

void draw_knight(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 4, p[1], "         {}         ");
    draw_line(x + 1, y + 5, p[1], "         {}         ");
    draw_line(x + 1, y + 6, p[0], "        .--.        ");
    draw_line(x + 1, y + 7, p[0], "       /.--.\\       ");
    draw_line(x + 1, y + 8, p[0], "       |====|       ");
    draw_line(x + 1, y + 9, p[0], "       |`::`|       ");
    draw_line(x + 1, y + 10, p[0], "   .-;`\\..../`;_.-^-");
    draw_line(x + 1, y + 11, p[0], "  /  |...::..|`   : ");
    draw_line(x + 1, y + 12, p[0], " |   /'''::''|   .:.");
}

/*=========================================================
    Commander
=========================================================*/

void draw_commander(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[0], "      ___");
    draw_line(x + 1, y + 4, p[0], "    ,` -.)          ");
    draw4(x + 1, y + 5, p[0], "   ( _/", p[1], "-", p[0], "\\\\", p[1], "-._       ");
    draw(x + 1, y + 6, p[0], "  /,", p[1], "|`--._,-^|      ");
    draw(x + 1, y + 7, p[0], "  \\_", p[1], "| |`-._/||      ");
    draw_line(x + 1, y + 8, p[1], "    |  `-, / |      ");
    draw_line(x + 1, y + 9, p[1], "    |     || |      ");
    draw_line(x + 1, y + 10, p[1], "     `r-._||/   __  ");
    draw_line(x + 1, y + 11, p[1], " __,-<_     )`-/  `.");
    draw_line(x + 1, y + 12, p[1], "'  \\   `---'   \\    ");
}

/*=========================================================
    Archer
=========================================================*/

void draw_archer(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[1], "            /\\.");
    draw_line(x + 1, y + 4, p[1], "           /   :.");
    draw_line(x + 1, y + 5, p[1], "          /     \\\\");
    draw4(x + 1, y + 6, p[0], "       ,;", p[1], "/", p[0], ",", p[1], "      ::");
    draw4(x + 1, y + 7, p[0], "   ___:c", p[1], "/", p[0], ".(      ", p[1], "||");
    draw3(x + 1, y + 8, p[0], " ,'  _|:)>>>--,-'", p[1], "B)", p[0], ">");
    draw4(x + 1, y + 9, p[0], "(  '---'", p[1], "\\", p[0], "--'` ", p[1], "_,'||");
    draw(x + 1, y + 10, p[0], " `--.    ", p[1], "\\ ,-'   ;;");
    draw(x + 1, y + 11, p[0], "     |    ", p[1], "\\|    //");
    draw(x + 1, y + 12, p[0], "     |     ", p[1], "\\   ;'");
}

/*=========================================================
    King
=========================================================*/

void draw_king(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 2, y + 3, p[2], "       .+. ");
    draw5(x + 2, y + 4, p[2], "    (^", p[1], "\\/", p[2], "^", p[1], "\\/", p[2], "^)");
    draw7(x + 2, y + 5, p[2], "     \\", p[3], "@", p[1], "*", p[4], "@", p[1], "*", p[3], "@", p[2], "/");
    draw_line(x + 2, y + 6, p[2], "     {_____}");
    draw_line(x + 2, y + 7, p[0], "    ///\"\"\"\\\\\\");
    draw_line(x + 2, y + 8, p[0], "    (/6   6\\)");
    draw_line(x + 2, y + 9, p[0], "     ||=^=||");
    draw_line(x + 2, y + 10, p[0], "     \\\\\\\\///");
    draw_line(x + 2, y + 11, p[0], "      \\\\///");
}

/*=========================================================
    Vampire
=========================================================*/

void draw_vampire(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 4, p[0], "| (   \\  /____\\__\\");
    draw4(x + 1, y + 5, p[0], " \\ \\      (_", p[1], "()", p[0], " (_", p[1], "()");
    draw_line(x + 1, y + 6, p[0], "  \\ \\            ._");
    draw_line(x + 1, y + 7, p[0], "   \\               \\");
    draw_line(x + 1, y + 8, p[0], "/\\ |`       (______/");
    draw_line(x + 1, y + 9, p[0], "  \\|     /\\___/");
    draw(x + 1, y + 10, p[0], "   \\     \\||", p[2], "VV");
    draw3(x + 1, y + 11, p[0], "    \\     \\|", p[2], "\"\"\"\"", p[0], ",");
    draw_line(x + 1, y + 12, p[0], "     \\     ______)");
}

/*=========================================================
    Vampiric Rite
=========================================================*/

void draw_mouth(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[0], "       __   __");
    draw_line(x + 1, y + 4, p[0], "    .-'  \".\"  '-.");
    draw_line(x + 1, y + 5, p[0], "  .'   ___,___   '.");
    draw3(x + 1, y + 6, p[0], " ;__.-;", p[1], " | | | ", p[0], ";-.__;");
    draw3(x + 1, y + 7, p[0], " |", p[1], " \\  | | | | |  / ", p[0], "|");
    draw3(x + 1, y + 8, p[0], "  \\ ", p[1], "\\/`\"`\"`\"`\"`\\/ ", p[0], "/");
    draw3(x + 1, y + 9, p[0], "   \\", p[1], "_.-,-,-,-,-._", p[0], "/");
    draw3(x + 1, y + 10, p[0], "    \\", p[1], "`-:_|_|_:-'", p[0], "/");
    draw_line(x + 1, y + 11, p[0], "     '._______.'");
}

/*=========================================================
    Lightning Bolt
=========================================================*/

void draw_lightning(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[0], "     (__         __)");
    draw_line(x + 1, y + 4, p[0], "       (_________)");
    draw_line(x + 1, y + 5, p[1], "         _/  /");
    draw_line(x + 1, y + 6, p[1], "        /  _/");
    draw_line(x + 1, y + 7, p[1], "      _/  /");
    draw_line(x + 1, y + 8, p[1], "     / __/");
    draw_line(x + 1, y + 9, p[1], "   _/ /");
    draw_line(x + 1, y + 10, p[1], "  /__/");
    draw_line(x + 1, y + 11, p[1], " //");
    draw_line(x + 1, y + 12, p[1], "/'");
}

/*=========================================================
    Grove
=========================================================*/

void draw_grove(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 3, p[1], "__.___\\\\, \\\\/ -.\\||");
    draw_line(x + 1, y + 4, p[1], "  `7-,--.`._||  / /");
    draw_line(x + 1, y + 5, p[1], "  /'     `-. `./ / |");
    draw(x + 1, y + 6, p[2], "  oooooooooo", p[1], "|    |//");
    draw(x + 1, y + 7, p[2], " ooooooooooooo", p[1], "    /");
    draw(x + 1, y + 8, p[2], "   oooooooooo", p[1], "-   |");
    draw_line(x + 1, y + 9, p[1], "            |   =|");
    draw_line(x + 1, y + 10, p[1], "            |    |");
    draw3(x + 1, y + 11, p[3], "-----------", p[1], "/ ,  . ", p[3], "-");
}

/*=========================================================
    Snow Storm
=========================================================*/

void draw_snow(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw3(x + 1, y + 3, p[1], "*       ", p[2], ".        ", p[1], "*");
    draw3(x + 1, y + 4, p[2], "    .    ", p[3], "*    ", p[2], ".");
    draw(x + 1, y + 5, p[1], "        *       ", p[2], ".");
    draw3(x + 1, y + 6, p[3], "   *    ", p[2], ".      ", p[1], "*");
    draw(x + 1, y + 7, p[2], "       .     ", p[1], "*     *");
    draw3(x + 1, y + 8, p[2], "  .       ", p[3], "*       ", p[2], ".");
    draw(x + 1, y + 9, p[1], "       *       ", p[2], ".  ");
    draw3(x + 1, y + 10, p[1], "*       ", p[2], ".      ", p[3], "*   ");
    draw3(x + 1, y + 11, p[2], "   .   ", p[3], "*       ", p[2], ".");
    draw(x + 1, y + 12, p[2], "       .      ", p[1], "* ");
}

/*=========================================================
    Graveyard
=========================================================*/

void draw_graveyard(
    int x,
    int y,
    const Card *card)
{
    const Color *p = card->palette;

    draw_line(x + 1, y + 5, p[0], "      .-'~~~`-.");
    draw_line(x + 1, y + 6, p[0], "    .'         `.");
    draw3(x + 1, y + 7, p[0], "    |  ", p[1], "R  I  P  ", p[0], "|");
    draw_line(x + 1, y + 8, p[0], "    |           |");
    draw_line(x + 1, y + 9, p[0], "    |           |");
    draw3(x + 1, y + 10, p[2], "  \\\\", p[0], "|           |", p[2], "//");
    draw_line(x + 1, y + 11, p[2], "^^^^^^^^^^^^^^^^^^^^");
}

/*=========================================================
    Dispatcher
=========================================================*/

void draw_image(
    int x,
    int y,
    const Card *card)
{
    draw_empty(x, y, card);
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

    // Soldier
    case 5:
        draw_soldier(x, y, card);
        break;

    // Thief
    case 6:
        draw_thief(x, y, card);
        break;

    // Particles (Nature's Blessing)
    case 7:
        draw_particles(x, y, card);
        break;

    case 8:
        draw_commander(x, y, card);
        break;

    // Knight
    case 9:
        draw_knight(x, y, card);
        break;

    // Embers
    case 10:
        draw_embers(x, y, card);
        break;

    // Archer
    case 11:
        draw_archer(x, y, card);
        break;

    // King
    case 12:
        draw_king(x, y, card);
        break;

    // Spear Goblin
    case 13:
        draw_spear_goblin(x, y, card);
        break;

    // Goblin Shaman
    case 14:
        draw_goblin_shaman(x, y, card);
        break;

    // Horn
    case 15:
        draw_horn(x, y, card);
        break;

    // Goblin Chieftain
    case 16:
        draw_goblin_chief(x, y, card);
        break;

    case 17:
        draw_mana_hoarder(x, y, card);
        break;

    case 18:
        draw_grove(x, y, card);
        break;

    case 19:
        draw_snow(x, y, card);
        break;

    case 20:
        draw_skeleton(x, y, card);
        break;

    case 21:
        draw_graveyard(x, y, card);
        break;

    case 22:
        draw_question(x, y, card);
        break;

    case 23:
        draw_mist(x, y, card);
        break;

    case 24:
        draw_vampire(x, y, card);
        break;

    case 25:
        draw_lightning(x, y, card);
        break;

    case 26:
        draw_mouth(x, y, card);
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

    /*
        Current Y position within the visible terminal.
    */

    y = 1;

    for (row = 0;
         row * per_row < db->card_count;
         row++)
    {
        /*
            If this entire row would extend below the
            terminal, naturally scroll the terminal.

            A card occupies through:

                y + CHEIGHT + 1
        */

        if (y + CHEIGHT + 1 > terminal_height)
        {
            int lines =
                (y + CHEIGHT + 1) - terminal_height;

            /*
                Move the cursor to the bottom and print
                newlines. This causes the terminal itself
                to scroll.
            */

            gotoxy(
                1,
                terminal_height);

            for (int j = 0; j < lines; j++)
                putchar('\n');

            /*
                The newly visible area begins at the
                bottom minus the number of lines that
                were scrolled.

                Recalculate the Y coordinate.
            */

            y -= lines;

            if (y < 1)
                y = 1;
        }

        /*
            Draw cards in this row.
        */

        for (col = 0;
             col < per_row;
             col++)
        {
            i = row * per_row + col;

            if (i >= db->card_count)
                break;

            x =
                3 +
                col * (CLENGTH + CARD_SPACING);

            draw_card(
                x,
                y,
                &db->cards[i]);
        }

        /*
            Move to the next row.
        */

        y += CHEIGHT + 3;
    }

    /*
        Leave cursor below the cards.
    */

    if (y > terminal_height)
        y = terminal_height;

    gotoxy(
        1,
        y);

    reset_color();
    show_cursor();
}