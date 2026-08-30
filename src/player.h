#ifndef PLAYER_H
#define PLAYER_H

#include "deck.h"
#include "board_art.h"

/*=========================================================
    Constants
=========================================================*/

#define MAX_PLAYER_NAME 256
#define MAX_HAND_SIZE  HAND_SLOTS

#define PLAYER_STARTING_HP    20
#define PLAYER_STARTING_MANA   100


/*=========================================================
    Player
=========================================================*/

typedef struct
{
    char name[MAX_PLAYER_NAME];

    int hp;
    int mana;

    Deck deck;

    Card *hand[MAX_HAND_SIZE];

    /*
        Index of the next card to draw from the deck.
    */
    int deck_position;

} Player;


/*=========================================================
    Initialization
=========================================================*/

void init_player(
    Player *player,
    const char *name);


/*=========================================================
    Deck
=========================================================*/

void add_deck_to_player(
    Player *player,
    const Deck *deck);


/*=========================================================
    Drawing
=========================================================*/

/*
    Draws one card from the player's deck into their hand.

    Returns:
        1 = success
        0 = failure
*/
int draw_card_to_hand(
    Player *player);


/*
    Draws multiple cards.

    Returns the number of cards successfully drawn.
*/
int draw_cards_to_hand(
    Player *player,
    int amount);

#endif