#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "access.h"
#include "player.h"
#include "deck.h"
#include "board_art.h"
#include "card_artwork.h"
#include "game_state.h"

#define CARD_DATABASE_FILE "src/card_info.json"
#define DECK_DIRECTORY     "decks"

int main(void)
{
    Deck selected_deck;

    /*
        Seed random number generator.
    */
    srand((unsigned int)time(NULL));

    /*
        Initialize game and players.
    */
    CardDatabase cards;
    DeckDatabase decks;
    GameState game;
    init_game_state_with_players(&game, "Coninhas", "Coninhas2");

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

    selected_deck = decks.decks[selected];

    /*
        Start with a clean screen.
    */
    clear_screen();

    /*
        Draw board.
    */
    draw_board(NULL);

    /*
        Give selected deck to player.
    */
    add_deck_to_player(
        &((&game)->players[0]),
        &selected_deck);

    add_deck_to_player(&((&game)->players[1]), &selected_deck);

    /*
        Shuffle runtime copy.
    */
    shuffle_deck(
        &((&game)->players[0]).deck);

        getchar();

    /*
        Opening hand.
    */
    draw_cards_to_hand(
        &((&game)->players[0]),
        3);

    main_phase(&game, 0);

    /*
        Keep terminal open.
    */
    gotoxy(1, 32);

    return 0;
}