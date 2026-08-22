#include <stdio.h>

#include "access.h"
#include "cards.h"
#include "deck.h"


#define CARD_DATABASE_FILE "src/card_info.json"
#define DECK_DIRECTORY     "decks"


int main(void)
{
    CardDatabase cards;
    DeckDatabase decks;

    int selected;


    /*-----------------------------------------------------
        Load card database
    -----------------------------------------------------*/

    printf(
        "Loading card database...\n");

    if (!load_cards(
            &cards,
            CARD_DATABASE_FILE))
    {
        printf(
            "ERROR: could not load '%s'.\n",
            CARD_DATABASE_FILE);

        return 1;
    }

    printf(
        "Loaded %d cards.\n",
        cards.card_count);


    /*-----------------------------------------------------
        Load decks
    -----------------------------------------------------*/

    printf(
        "Loading decks from '%s'...\n",
        DECK_DIRECTORY);

    if (!load_decks(
            &decks,
            &cards,
            DECK_DIRECTORY))
    {
        printf(
            "ERROR: could not load deck directory.\n");

        return 1;
    }

    printf(
        "Loaded %d valid deck(s).\n",
        decks.deck_count);

    if (decks.deck_count == 0)
    {
        printf(
            "No valid decks found.\n");

        return 1;
    }


    /*-----------------------------------------------------
        Selector
    -----------------------------------------------------*/

    selected =
        deck_selector(
            &decks);


    /*-----------------------------------------------------
        Result
    -----------------------------------------------------*/

    if (selected < 0)
    {
        printf(
            "Deck selection cancelled.\n");

        return 0;
    }

    printf("\n");

    printf(
        "Selected deck: %s\n",
        decks.decks[selected].name);

    printf(
        "Front card: %s\n",
        decks.decks[selected].front_card->name);

    printf(
        "Deck size: %d\n",
        DECK_SIZE);


    return 0;
}