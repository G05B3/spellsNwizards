#include "board_art.h"

#include <stdio.h>

#include "access.h"

/*=========================================================*
    Configuration
=========================================================*/

#define CREATURE_SLOTS     4
#define HAND_SLOTS         4
#define STACK_SLOTS        3

#define REGION_SPACING     2

/*
    Width of one card slot.

    This follows the spacing convention already used
    by the card viewer / old board.
*/
#define SLOT_WIDTH          (CLENGTH + 7)

/*
    Height of one card slot.
*/
#define SLOT_HEIGHT         (CHEIGHT + 2)

/*
    Main board has:

        [ ARTIFACT ][ CREATURE ][ CREATURE ][ CREATURE ][ CREATURE ]
*/
#define BOARD_SLOTS         (CREATURE_SLOTS + 1)

#define BOARD_WIDTH         (SLOT_WIDTH * BOARD_SLOTS - 1)

#define STACK_WIDTH         CLENGTH

/*
    Two creature rows + one hand row.
*/
#define BOARD_HEIGHT        (SLOT_HEIGHT * 3 - 1)


/*=========================================================*
    Generic Box
=========================================================*/

static void draw_box(
    int x,
    int y,
    int width,
    int height,
    Color color)
{
    int i;

    set_color(color);

    /*
        Top.
    */
    gotoxy(x, y);

    printf("%s", LTL);

    for (i = 0; i < width; i++)
        printf("%s", LHZ);

    printf("%s", LTR);

    /*
        Sides.
    */
    for (i = 1; i <= height; i++)
    {
        gotoxy(x, y + i);
        printf("%s", LVL);

        gotoxy(x + width + 1, y + i);
        printf("%s", LVL);
    }

    /*
        Bottom.
    */
    gotoxy(x, y + height + 1);

    printf("%s", LBL);

    for (i = 0; i < width; i++)
        printf("%s", LHZ);

    printf("%s", LBR);

    reset_color();
}


/*=========================================================*
    Centered Label
=========================================================*/

static void draw_label(
    int x,
    int y,
    int width,
    const char *text,
    Color color)
{
    int len = 0;

    while (text[len] != '\0')
        len++;

    set_color(color);

    gotoxy(
        x + width / 2 - len / 2,
        y);

    printf("%s", text);

    reset_color();
}


/*=========================================================*
    Board Row
=========================================================*/

/*
    Draws:

        ┌─────────┬─────────┬─────────┬─────────┬─────────┐
        │         │         │         │         │         │
        │         │         │         │         │         │
        └─────────┴─────────┴─────────┴─────────┴─────────┘

        The first slot is the artifact slot.
        The remaining four are creature slots.
*/
static void draw_board_row(
    int x,
    int y,
    Color color)
{
    int i;
    int j;

    set_color(color);

    /*
        Top.
    */
    gotoxy(x, y);

    printf("%s", LTL);

    for (i = 0; i < BOARD_WIDTH; i++)
        printf("%s", LHZ);

    printf("%s", LTR);

    /*
        Vertical sides.
    */
    for (i = 1; i <= CHEIGHT + 1; i++)
    {
        gotoxy(x, y + i);
        printf("%s", LVL);

        gotoxy(
            x + BOARD_WIDTH + 1,
            y + i);

        printf("%s", LVL);
    }

    /*
        Internal separators.
    */
    for (i = 1; i < BOARD_SLOTS; i++)
    {
        gotoxy(
            x + SLOT_WIDTH * i,
            y);

        printf("%s", LC1);

        gotoxy(
            x + SLOT_WIDTH * i,
            y + CHEIGHT + 2);

        printf("%s", LC2);

        for (j = 1; j <= CHEIGHT + 1; j++)
        {
            gotoxy(
                x + SLOT_WIDTH * i,
                y + j);

            printf("%s", LVL);
        }
    }

    /*
        Bottom.
    */
    gotoxy(
        x,
        y + CHEIGHT + 2);

    printf("%s", LBL);

    for (i = 0; i < BOARD_WIDTH; i++)
        printf("%s", LHZ);

    printf("%s", LBR);

    reset_color();
}


/*=========================================================*
    Player Hand
=========================================================*/

static void draw_hand(
    int x,
    int y)
{
    int i;
    int j;
    int width;

    width =
        HAND_SLOTS * SLOT_WIDTH - 1;

    set_color(GREEN);

    /*
        Top.
    */
    gotoxy(x, y);

    printf("%s", LTL);

    for (i = 0; i < width; i++)
        printf("%s", LHZ);

    printf("%s", LTR);

    /*
        Sides.
    */
    for (i = 1; i <= CHEIGHT + 1; i++)
    {
        gotoxy(x, y + i);
        printf("%s", LVL);

        gotoxy(
            x + width + 1,
            y + i);

        printf("%s", LVL);
    }

    /*
        Internal separators.
    */
    for (i = 1; i < HAND_SLOTS; i++)
    {
        gotoxy(
            x + SLOT_WIDTH * i,
            y);

        printf("%s", LC1);

        gotoxy(
            x + SLOT_WIDTH * i,
            y + CHEIGHT + 2);

        printf("%s", LC2);

        for (j = 1; j <= CHEIGHT + 1; j++)
        {
            gotoxy(
                x + SLOT_WIDTH * i,
                y + j);

            printf("%s", LVL);
        }
    }

    /*
        Bottom.
    */
    gotoxy(
        x,
        y + CHEIGHT + 2);

    printf("%s", LBL);

    for (i = 0; i < width; i++)
        printf("%s", LHZ);

    printf("%s", LBR);

    reset_color();
}


/*=========================================================*
    Reaction Stack
=========================================================*/

static void draw_stack(
    int x,
    int y)
{
    int i;

    /*
        Label.
    */
    draw_label(
        x,
        y - 2,
        STACK_WIDTH,
        "REACTION STACK",
        MAGENTA);

    /*
        Three card-sized slots.
    */
    for (i = 0; i < STACK_SLOTS; i++)
    {
        draw_box(
            x,
            y + i * SLOT_HEIGHT,
            STACK_WIDTH,
            CHEIGHT + 1,
            MAGENTA);
    }
}


/*=========================================================*
    Round Box
=========================================================*/

static void draw_round_box(
    int x,
    int y,
    int width,
    int height)
{
    draw_box(
        x,
        y,
        width,
        height,
        ORANGE);

    draw_label(
        x,
        y + 1,
        width,
        "ROUND",
        ORANGE);

    draw_label(
        x,
        y + 3,
        width,
        "NO. 1",
        WHITE);
}


/*=========================================================*
    Card Inspection
=========================================================*/

static void draw_inspection_box(
    int x,
    int y,
    int width,
    int height)
{
    int card_width;
    int card_height;

    int text_x;
    int text_y;

    draw_box(
        x,
        y,
        width,
        height,
        BLUE);

    draw_label(
        x,
        y - 2,
        width,
        "CARD INSPECTION",
        BLUE);

    /*
        Placeholder card.

        Eventually this will simply call draw_card()
        for the currently selected card.
    */
    card_width = CLENGTH;
    card_height = CHEIGHT + 1;

    draw_box(
        x + 2,
        y + 2,
        card_width,
        card_height,
        WHITE);

    draw_label(
        x + 2,
        y + 4,
        card_width,
        "CARD",
        WHITE);

    /*
        Placeholder information.
    */
    text_x = x + card_width + 6;
    text_y = y + 2;

    set_color(WHITE);

    gotoxy(text_x, text_y);
    printf("Name: Warrior");

    gotoxy(text_x, text_y + 2);
    printf("Type: Creature");

    gotoxy(text_x, text_y + 3);
    printf("Race: Human");

    gotoxy(text_x, text_y + 4);
    printf("Cost: 2");

    gotoxy(text_x, text_y + 5);
    printf("ATK: 2");

    gotoxy(text_x, text_y + 6);
    printf("HP:  3");

    gotoxy(text_x, text_y + 8);
    printf("Abilities:");

    gotoxy(text_x + 2, text_y + 9);
    printf("First Strike");

    gotoxy(text_x + 2, text_y + 10);
    printf("Guardian");

    /*
        Description section.
    */
    gotoxy(
        x + 2,
        y + card_height + 5);

    printf("Description:");

    gotoxy(
        x + 2,
        y + card_height + 7);

    printf(
        "This brave warrior fights for you");

    gotoxy(
        x + 2,
        y + card_height + 8);

    printf(
        "with every ounce of strength he has!");

    reset_color();
}


/*=========================================================*
    Main Board
=========================================================*/

void draw_board(void)
{
    int stack_x;
    int board_x;
    int info_x;

    int opponent_y;
    int player_y;
    int hand_y;

    int round_y;

    /*
        ---------------------------------------------------
        Horizontal layout

        STACK | MAIN BOARD | INFORMATION
        ---------------------------------------------------
    */

    stack_x = 2;

    board_x =
        stack_x +
        STACK_WIDTH +
        REGION_SPACING +
        2;

    info_x =
        board_x +
        BOARD_WIDTH +
        REGION_SPACING - 1;


    /*
        ---------------------------------------------------
        Vertical layout
        ---------------------------------------------------
    */

    opponent_y = 4;

    player_y =
        opponent_y +
        SLOT_HEIGHT +
        3;

    hand_y =
        player_y +
        SLOT_HEIGHT +
        3;


    /*
        ---------------------------------------------------
        Reaction stack
        ---------------------------------------------------
    */

    draw_stack(
        stack_x,
        opponent_y);


    /*
        ---------------------------------------------------
        Opponent
        ---------------------------------------------------
    */

    draw_board_row(
        board_x-1,
        opponent_y,
        RED);


    /*
        ---------------------------------------------------
        Player
        ---------------------------------------------------
    */

    draw_board_row(
        board_x-1,
        player_y-3,
        GREEN);


    /*
        ---------------------------------------------------
        Hand

        Notice that it starts one slot to the right.
        The empty space corresponds to the artifact /
        board-side alignment and keeps the visual layout
        consistent with the intended design.
        ---------------------------------------------------
    */

    draw_hand(
        board_x + SLOT_WIDTH - 1,
        hand_y-5);


    /*
        ---------------------------------------------------
        Artifact / creature labels
        ---------------------------------------------------
    */

    draw_label(
        board_x,
        opponent_y + CHEIGHT + 6,
        SLOT_WIDTH - 1,
        "ARTIFACT",
        ORANGE);

    for (int i = 0;
         i < CREATURE_SLOTS;
         i++)
    {
        draw_label(
            board_x + SLOT_WIDTH * (i + 1),
            opponent_y + CHEIGHT + 6,
            SLOT_WIDTH - 1,
            "CREATURE",
            CYAN);
    }


    /*
        ---------------------------------------------------
        Right-side information
        ---------------------------------------------------
    */

    round_y = opponent_y;

    draw_round_box(
        info_x,
        round_y,
        20,
        5);

    draw_inspection_box(
        info_x,
        player_y+1,
        46,
        24);


    /*
        Put cursor below everything.
    */

    gotoxy(
        1,
        player_y + 27);

    reset_color();
}
