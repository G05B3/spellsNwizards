#ifndef BOARD_ART_H
#define BOARD_ART_H

#include "access.h"

/*=========================================================
    Board Sides
=========================================================*/

#define OPPONENT_SIDE 0
#define PLAYER_SIDE   1

#define ARTIFACT_SLOT       0
#define FIRST_CREATURE_SLOT 1


#define CREATURE_SLOTS 4
#define HAND_SLOTS 4
#define STACK_SLOTS 3

#define REGION_SPACING 2

/*
    Width of one card slot.

    This follows the spacing convention already used
    by the card viewer / old board.
*/
#define SLOT_WIDTH (CLENGTH + 7)

/*
    Height of one card slot.
*/
#define SLOT_HEIGHT (CHEIGHT + 2)

/*
    Main board has:

        [ ARTIFACT ][ CREATURE ][ CREATURE ][ CREATURE ][ CREATURE ]
*/
#define BOARD_SLOTS (CREATURE_SLOTS + 1)

#define BOARD_WIDTH (SLOT_WIDTH * BOARD_SLOTS - 1)

#define STACK_WIDTH SLOT_WIDTH - 1

/*
    Two creature rows + one hand row.
*/
#define BOARD_HEIGHT (SLOT_HEIGHT * 3 - 1)


/*=========================================================
    Board Position
=========================================================*/

typedef struct
{
    int x;
    int y;
} BoardPosition;


/*=========================================================
    Board Positions
=========================================================*/

/*
    Main board:

        [ A ][ C ][ C ][ C ][ C ]  <- opponent
        [ A ][ C ][ C ][ C ][ C ]  <- player
*/

extern BoardPosition board_slots[2][BOARD_SLOTS];


/*
    Player hand:

        [ C ][ C ][ C ][ C ]
*/

extern BoardPosition hand_slots[HAND_SLOTS];


/*
    Reaction stack:

        [ S ]  <- opponent
        [ S ]  <- player
        [ S ]  <- hand-side
*/

extern BoardPosition stack_slots[STACK_SLOTS];

extern BoardPosition environment_slot;

/*=========================================================
    Board Drawing
=========================================================*/

void draw_board(Color* c);

#endif