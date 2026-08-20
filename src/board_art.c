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

static void draw_box_double(
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

    printf("%s", TL);

    for (i = 0; i < width; i++)
        printf("%s", HZ);

    printf("%s", TR);

    /*
        Sides.
    */
    for (i = 1; i <= height; i++)
    {
        gotoxy(x, y + i);
        printf("%s", VL);

        gotoxy(x + width + 1, y + i);
        printf("%s", VL);
    }

    /*
        Bottom.
    */
    gotoxy(x, y + height + 1);

    printf("%s", BL);

    for (i = 0; i < width; i++)
        printf("%s", HZ);

    printf("%s", BR);

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
/*=========================================================*
    Main Board
*=========================================================*/

/*
    Draws the complete 2 x 5 board:

        ┌─────┬─────┬─────┬─────┬─────┐
        │     │     │     │     │     │
        │     │     │     │     │     │
        ├─────┼─────┼─────┼─────┼─────┤
        │     │     │     │     │     │
        │     │     │     │     │     │
        └─────┴─────┴─────┴─────┴─────┘

    The first column is the artifact slot.
    The remaining four columns are creature slots.

    The middle separator is shared between the
    opponent and player sides.
*/

static void draw_board_grid(
    int x,
    int y,
    Color opponent_color,
    Color player_color)
{
    int i;
    int j;

    /*
        ---------------------------------------------------
        Top border
        ---------------------------------------------------
    */

    set_color(opponent_color);

    gotoxy(x, y);
    printf("%s", LTL);

    for (i = 0; i < BOARD_SLOTS; i++)
    {
        for (j = 0; j < SLOT_WIDTH - 1; j++)
            printf("%s", LHZ);

        if (i < BOARD_SLOTS - 1)
            printf("%s", LC1);
    }

    printf("%s", LTR);


    /*
        ---------------------------------------------------
        Vertical separators — upper half
        ---------------------------------------------------
    */

    for (i = 1; i < SLOT_HEIGHT; i++)
    {
        gotoxy(x, y + i);
        printf("%s", LVL);

        for (j = 1; j < BOARD_SLOTS; j++)
        {
            gotoxy(
                x + SLOT_WIDTH * j,
                y + i);

            printf("%s", LVL);
        }

        gotoxy(
            x + BOARD_WIDTH + 1,
            y + i);

        printf("%s", LVL);
    }


    /*
        ---------------------------------------------------
        Middle separator
        ---------------------------------------------------
    */

    set_color(player_color);

    gotoxy(
        x,
        y + SLOT_HEIGHT);

    printf("%s", LD1);

    for (i = 0; i < BOARD_SLOTS; i++)
    {
        for (j = 0; j < SLOT_WIDTH - 1; j++)
            printf("%s", LHZ);

        if (i < BOARD_SLOTS - 1)
            printf("%s", LCR);
    }

    printf("%s", LD2);


    /*
        ---------------------------------------------------
        Vertical separators — lower half
        ---------------------------------------------------
    */

    for (i = SLOT_HEIGHT + 1;
         i < SLOT_HEIGHT * 2;
         i++)
    {
        gotoxy(x, y + i);
        printf("%s", LVL);

        for (j = 1; j < BOARD_SLOTS; j++)
        {
            gotoxy(
                x + SLOT_WIDTH * j,
                y + i);

            printf("%s", LVL);
        }

        gotoxy(
            x + BOARD_WIDTH + 1,
            y + i);

        printf("%s", LVL);
    }


    /*
        ---------------------------------------------------
        Bottom border
        ---------------------------------------------------
    */

    gotoxy(
        x,
        y + SLOT_HEIGHT * 2);

    printf("%s", LBL);

    for (i = 0; i < BOARD_SLOTS; i++)
    {
        for (j = 0; j < SLOT_WIDTH - 1; j++)
            printf("%s", LHZ);

        if (i < BOARD_SLOTS - 1)
            printf("%s", LC2);
    }

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

    set_color(CYAN);

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

    draw_box_double(x, y - 3, STACK_WIDTH, 1, MAGENTA);

    /*
        Label.
    */
    draw_label(
        x + 1,
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
        
        if (i > 0)
        {
            set_color(MAGENTA);
            gotoxy(x, y + i * SLOT_HEIGHT);
            printf("%s", LD1);
            gotoxy(x + STACK_WIDTH + 1, y + i * SLOT_HEIGHT);
            printf("%s", LD2);
        }
    }
    reset_color();
}

/*=========================================================*
    Event / Message Box
*=========================================================*/

/*
    Small double-line box beside the reaction-stack header.

    It spans the width of the complete 5-slot board and is
    intended for temporary game messages later.

        ╔══════════════════════════════════════════════════╗
        ║                                                  ║
        ╚══════════════════════════════════════════════════╝
*/

static void draw_event_box(
    int x,
    int y)
{
    draw_box_double(
        x,
        y,
        BOARD_WIDTH,
        1,
        LIGHT_YELLOW);
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
    set_color(GRAY);
    for (int i = 0; i < y; i++)
    {
        gotoxy(x + 2, i);
        if (i % 2 == 0)
            printf("%s", VL);
        else
            printf("%s", LVL);

        gotoxy(x + width - 1, i);
        if (i % 2 == 0)
            printf("%s", VL);
        else
            printf("%s", LVL);


    }
    draw_box(
        x,
        y,
        width,
        height,
        ORANGE);

    set_color(ORANGE);
    gotoxy(x + 2, y);
    printf("%s", LC2);
    gotoxy(x + width - 1, y);
    printf("%s", LC2);

    draw_label(
        x,
        y + 1,
        width,
        "ROUND",
        ORANGE);

    draw_label(
        x,
        y + 2,
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
        1;

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

    draw_event_box(
        board_x - 1,
        opponent_y - 3);

    draw_board_grid(
        board_x-1,
        opponent_y,
        WHITE, WHITE);

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

    round_y = player_y - 8;

    draw_round_box(
        info_x,
        round_y,
        20,
        2);

    gotoxy(info_x, player_y - 4);
    printf("Opponent's Side");
    gotoxy(info_x, player_y - 3);
    set_color(RED);
    for (int i = 0; i < 22; i++)
        printf("-");
    reset_color();
    gotoxy(info_x, player_y - 2);
    printf("Your Side");


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
