#include "board_art.h"

#include <stdio.h>
#include <string.h>

#include "access.h"

/*=========================================================*
    Configuration
=========================================================*/

BoardPosition board_slots[2][BOARD_SLOTS];
BoardPosition hand_slots[HAND_SLOTS];
BoardPosition stack_slots[STACK_SLOTS];
BoardPosition environment_slot;

static int event_x;
static int event_y;

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

    for (i = 1; i <= SLOT_HEIGHT; i++)
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
        y + SLOT_HEIGHT + 1);

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

    for (i = SLOT_HEIGHT + 2;
         i < SLOT_HEIGHT * 2 + 2;
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
        y + SLOT_HEIGHT * 2 + 2);

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
    for (i = 1; i <= SLOT_HEIGHT; i++)
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

        for (j = 1; j <= SLOT_HEIGHT; j++)
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
        y + SLOT_HEIGHT + 1);

    printf("%s", LBL);

    for (i = 0; i < width; i++){
        if ((i+1) % SLOT_WIDTH == 0)
            printf("%s", LC2);
        else
            printf("%s", LHZ);
    }

    printf("%s", LBR);

    reset_color();
}

/*=========================================================*
    Environment Slot
=========================================================*/

static void draw_environment_slot(
    int x,
    int y)
{
    /*
        Name box
        -------------------------------------------------
        ╔════════════════════╗
        ║    ENVIRONMENT     ║
        ╚════════════════════╝
    */

    draw_box_double(
        x,
        y - 3,
        SLOT_WIDTH - 1,
        1,
        GREEN);

    draw_label(
        x + 1,
        y - 2,
        SLOT_WIDTH - 1,
        "ENVIRONMENT",
        GREEN);


    /*
        Card slot
        -------------------------------------------------
        ┌────────────────────┐
        │                    │
        │                    │
        │                    │
        │                    │
        │                    │
        │                    │
        │                    │
        │                    │
        │                    │
        │                    │
        │                    │
        │                    │
        └────────────────────┘
    */

    draw_box(
        x,
        y,
        SLOT_WIDTH - 1,
        SLOT_HEIGHT,
        WHITE);
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
        x + 2,
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
            y + i * SLOT_HEIGHT + i,
            STACK_WIDTH,
            SLOT_HEIGHT,
            MAGENTA);

        if (i > 0)
        {
            set_color(MAGENTA);
            gotoxy(x, y + i * SLOT_HEIGHT + i);
            printf("%s", LD1);
            gotoxy(x + STACK_WIDTH + 1, y + i * SLOT_HEIGHT + i);
            printf("%s", LD2);
        }
    }
    reset_color();
}

void draw_players_info(
    const char *player_one_name,
    int player_one_hp,
    int player_one_mana,
    const char *player_two_name,
    int player_two_hp,
    int player_two_mana)
{
    int x = 2;
    int y = 4 * SLOT_HEIGHT - 4;

    if (player_one_name == NULL || player_two_name == NULL)
    {
        draw_box_double(
            x,
            y,
            SLOT_WIDTH,
            3,
            LIGHT_YELLOW);

        return;
    }

    /*
        Adjust these dimensions/position to wherever your
        player information box is located.
    */
    draw_box_double(
        x,
        y,
        SLOT_WIDTH,
        3,
        LIGHT_YELLOW);

    /*
        Player one.
    */
    draw_text(
        x + 2,
        y + 1,
        WHITE,
        "%s  HP: %d  Mana: %d",
        player_one_name,
        player_one_hp,
        player_one_mana);

    /*
        Player two.
    */
    draw_text(
        x + 2, 
        y + 3,
        WHITE,
        "%s  HP: %d  Mana: %d",
        player_two_name,
        player_two_hp,
        player_two_mana);
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
    int y, Color* c)
{
    if (c == NULL)
        draw_box_double(x, y, BOARD_WIDTH, 1, LIGHT_YELLOW);
    else
        draw_box_double(x, y, BOARD_WIDTH, 1, *c);
}

void write_event(
    const char *text)
{
    char buffer[BOARD_WIDTH + 1];
    int text_length;
    int x;

    if (text == NULL)
        return;

    memset(
        buffer,
        ' ',
        BOARD_WIDTH);

    buffer[BOARD_WIDTH] = '\0';

    /*
        Clear event-box contents.
    */
    draw_line(
        event_x + 1,
        event_y + 1,
        LIGHT_YELLOW,
        buffer);

    text_length = strlen(text);

    if (text_length > BOARD_WIDTH)
        text_length = BOARD_WIDTH;

    x = event_x + 1 +
        (BOARD_WIDTH - text_length) / 2;

    /*
        Draw centered event.
    */
    draw_line(
        x,
        event_y + 1,
        LIGHT_YELLOW,
        text);
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
        y - 1,
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

void draw_board(Color* c)
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

    opponent_y = 5;

    player_y =
        opponent_y +
        SLOT_HEIGHT +
        4;

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
        opponent_y - 3, c);

    if (c == NULL)
        draw_board_grid(board_x - 1, opponent_y, WHITE, WHITE);
    else
        draw_board_grid(board_x - 1, opponent_y, *c, *c);

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
        board_x + SLOT_WIDTH - SLOT_WIDTH - 1,
        hand_y - 4);

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

    gotoxy(info_x, player_y - 3);
    printf("Opponent's Side");
    gotoxy(info_x, player_y - 2);
    set_color(RED);
    for (int i = 0; i < 22; i++)
        printf("-");
    reset_color();
    gotoxy(info_x, player_y - 1);
    printf("Your Side");

    draw_inspection_box(
        info_x,
        player_y,
        48,
        27);



        draw_environment_slot(
    info_x + 22,
    opponent_y);

    reset_color();

    /*
        ---------------------------------------------------
        Store board slot coordinates
        ---------------------------------------------------
    */

    /* Opponent board */
    for (int i = 0; i < BOARD_SLOTS; i++)
    {
        board_slots[OPPONENT_SIDE][i].x =
            board_x + SLOT_WIDTH * i;

        board_slots[OPPONENT_SIDE][i].y =
            opponent_y + 1;
    }

    /* Player board */
    for (int i = 0; i < BOARD_SLOTS; i++)
    {
        board_slots[PLAYER_SIDE][i].x =
            board_x + SLOT_WIDTH * i;

        board_slots[PLAYER_SIDE][i].y =
            player_y - 2;
    }

    /* Player hand */
    for (int i = 0; i < HAND_SLOTS; i++)
    {
        hand_slots[i].x =
            board_x + SLOT_WIDTH
            + SLOT_WIDTH * (i-1);

        hand_slots[i].y =
            hand_y - 3;
    }

    /* Reaction stack */
    for (int i = 0; i < STACK_SLOTS; i++)
    {
        stack_slots[i].x =
            stack_x + 1;

        stack_slots[i].y =
            opponent_y
            + i * SLOT_HEIGHT + i + 1;
    }

    /*
        Environment

        The environment is positioned to the right
        of the round counter, using the same x/y
        coordinates as the actual environment slot.
    */

    environment_slot.x =
        info_x + 23;

    environment_slot.y =
        opponent_y + 1;

    event_x = board_x - 1;
    event_y = opponent_y - 3;

    draw_players_info(NULL, 0, 0, NULL, 0, 0);
}
