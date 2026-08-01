#ifndef CARD_H
#define CARD_H

#include "access.h"
#include "cJSON.h"

/*=========================================================
    Constants
=========================================================*/

#define MAX_CARDS      1024


/*=========================================================
    Card Structure
=========================================================*/

typedef struct
{
    /*
        Assigned automatically when loading.

        Corresponds to the index inside
        card_info.json.
    */
    int id;

    /*
        Card name.
    */
    char name[256];

    /*
        Artwork index.
    */
    int image_id;

    /*
        Colors used by the artwork.

        Missing entries default to WHITE.
    */
    Color palette[PALETTE_SIZE];

} Card;


/*=========================================================
    Card Database
=========================================================*/

typedef struct
{
    Card cards[MAX_CARDS];

    int card_count;

} CardDatabase;


/*=========================================================
    Functions
=========================================================*/

/*
    Loads cards from a JSON file.

    Returns:

        1 = success

        0 = failure
*/

int load_cards(
    CardDatabase *db,
    const char *filename);


/*
    Optional helper.

    Returns NULL if the ID is invalid.
*/

Card *get_card(
    CardDatabase *db,
    int id);

#endif