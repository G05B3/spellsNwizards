#ifndef DECK_H
#define DECK_H

#include "cards.h"

/*=========================================================
    Deck Rules
=========================================================*/

#define DECK_SIZE              40

#define MAX_CREATURE_COPIES     10 // CHANGE TO 3
#define MAX_SPELL_COPIES        10 // CHANGE TO 3
#define MAX_ARTIFACT_COPIES     1
#define MAX_ENVIRONMENT_COPIES  1

#define MAX_DECK_NAME          128
#define MAX_DECKS             256

#define DEFAULT_DECK_PATH     "decks"


/*=========================================================
    Deck
=========================================================*/

typedef struct
{
    char name[MAX_DECK_NAME];

    /*
        Front card is resolved to an actual card from
        the card database.
    */
    Card *front_card;

    /*
        Actual cards in the deck.

        The same Card pointer may occur multiple times,
        representing multiple copies.
    */
    Card *cards[DECK_SIZE];

} Deck;


/*=========================================================
    Deck Database
=========================================================*/

typedef struct
{
    Deck decks[MAX_DECKS];

    int deck_count;

} DeckDatabase;


/*=========================================================
    Loading / Validation
=========================================================*/

/*
    Load every .json deck in a directory.

    Invalid decks are ignored.

    Returns 1 on success, 0 on failure.
*/
int load_decks(
    DeckDatabase *db,
    CardDatabase *cards,
    const char *path);


/*
    Load one deck from a JSON file.

    Returns 1 if the deck is valid.
    Returns 0 otherwise.
*/
int load_deck(
    Deck *deck,
    CardDatabase *cards,
    const char *filename);


/*
    Validate an already-loaded deck.

    Returns 1 if valid, 0 otherwise.
*/
int validate_deck(
    const Deck *deck);


/*=========================================================
    Deck Drawing
=========================================================*/

/*
    Draw a deck.

    selected = 1:
        Draw normally using the front card's palette.

    selected = 0:
        Draw using a grayscale dummy card.
*/
void draw_deck(
    int x,
    int y,
    const Deck *deck,
    int selected);


/*=========================================================
    Deck Selector
=========================================================*/

/*
    Display the interactive deck selector.

    Left / Right arrows or A / D:
        Move selection.

    Enter:
        Select current deck.

    Escape:
        Cancel.

    Returns:
        >= 0 : selected deck index
        -1   : cancelled / no selection
*/
int deck_selector(
    const DeckDatabase *db);


/*=========================================================
    Utilities
=========================================================*/

void free_decks(
    DeckDatabase *db);

#endif 