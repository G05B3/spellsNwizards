#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include <stdbool.h>

#include "cards.h"
#include "board_art.h"

/*=========================================================
    Constants
=========================================================*/

#define MAX_GRAVEYARD_SIZE 1024


/*=========================================================
    Card Instance
=========================================================*/

/*
    A Card describes the immutable definition of a card.

    A CardInstance describes a card that currently exists
    in the game and therefore needs mutable runtime state.
*/
typedef struct
{
    /*
        Original card definition.

        This points into the CardDatabase and is never owned
        by the instance.
    */
    Card *card;


    /*-----------------------------------------------------*
        Combat state
    *-----------------------------------------------------*/

    int atk;
    int hp;


    /*-----------------------------------------------------*
        Runtime boolean abilities
        -----------------------------------------------------

        These begin as copies of the Card's abilities, but
        can subsequently be changed by effects.
    *-----------------------------------------------------*/

    bool first_strike;
    bool guardian;
    bool healer;
    bool vampirism;
    bool mana_harvest;


    /*-----------------------------------------------------*
        Runtime parameterized abilities
    *-----------------------------------------------------*/

    int multiple_attacks;
    int mana_generation;
    int berserk;


    /*-----------------------------------------------------*
        Runtime status / attack effects
    *-----------------------------------------------------*/

    /*
        Current status counters.

        Example:

            current_status[STATUS_POISON] = 3

        means the creature currently has 3 poison counters.
    */
    int current_status[STATUS_COUNT];


    /*
        Runtime status immunities.

        These are copied from the Card definition and may
        subsequently be modified by effects.
    */
    bool status_immunity[STATUS_COUNT];


    /*
        Runtime attack-type immunities.
    */
    bool attack_immunity[ATTACK_TYPE_COUNT];


    /*
        Whether this instance currently occupies a board slot.
    */
    bool active;

} CardInstance;


/*=========================================================
    Graveyard
=========================================================*/

/*
    The graveyard is shared by both players.

    It behaves as a stack: the most recently added card is
    the top card.
*/
typedef struct
{
    Card *cards[MAX_GRAVEYARD_SIZE];

    int count;

} Graveyard;


/*=========================================================
    Board State
=========================================================*/

typedef struct
{
    /*
        Battlefield.

        Each player has:

            slot 0 = artifact
            slots 1-4 = creatures

        A slot may be inactive.
    */
    CardInstance cards[2][BOARD_SLOTS];

    CardInstance environment;


    /*
        Shared graveyard.
    */
    Graveyard graveyard;

} BoardState;


/*=========================================================
    Card Instance
=========================================================*/

/*
    Initialize a CardInstance from a Card definition.

    Runtime values are initialized from the card's printed
    values.
*/
void init_card_instance(
    CardInstance *instance,
    Card *card);


/*
    Clear a CardInstance so that its board slot becomes empty.
*/
void clear_card_instance(
    CardInstance *instance);


CardInstance *get_environment(
    BoardState *board);

/*=========================================================
    Board State
=========================================================*/

/*
    Initialize an empty board.
*/
void init_board_state(
    BoardState *board);


/*
    Returns a pointer to the CardInstance occupying a slot.

    Returns NULL if the slot is empty or invalid.
*/
CardInstance *get_board_instance(
    BoardState *board,
    int side,
    int slot);


/*
    Place a CardInstance into a board slot.

    The instance is copied into the board state.

    Returns:
        1 = success
        0 = failure
*/
int place_card_instance(
    BoardState *board,
    int side,
    int slot,
    const CardInstance *instance);


/*
    Remove a card from a board slot.

    The instance is copied into `out_instance` if it is
    non-NULL.

    Returns:
        1 = success
        0 = slot was empty / invalid
*/
int remove_card_instance(
    BoardState *board,
    int side,
    int slot,
    CardInstance *out_instance);


/*=========================================================
    Graveyard
=========================================================*/

/*
    Add a card to the top of the shared graveyard.

    Returns:
        1 = success
        0 = graveyard full
*/
int add_to_graveyard(
    BoardState *board,
    Card *card);


/*
    Remove and return the top card of the graveyard.

    Returns:
        Card * = card removed
        NULL  = graveyard empty
*/
Card *pop_graveyard(
    BoardState *board);


/*
    Peek at the top card without removing it.

    Returns NULL if empty.
*/
Card *peek_graveyard(
    const BoardState *board);

#endif