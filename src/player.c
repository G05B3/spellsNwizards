#include "player.h"

#include <string.h>

#include "board_art.h"
#include "card_artwork.h"


/*=========================================================
    Initialization
=========================================================*/

void init_player(
    Player *player,
    const char *name)
{
    int i;

    player->hp = PLAYER_STARTING_HP;
    player->mana = PLAYER_STARTING_MANA;

    player->deck_position = 0;

    /*
        Empty deck.
    */
    player->deck.name[0] = '\0';
    player->deck.front_card = NULL;

    for (i = 0; i < DECK_SIZE; i++)
        player->deck.cards[i] = NULL;

    /*
        Empty hand.
    */
    for (i = 0; i < MAX_HAND_SIZE; i++)
        player->hand[i] = NULL;

    /*
        Player name.
    */
    if (name != NULL)
    {
        strncpy(
            player->name,
            name,
            MAX_PLAYER_NAME - 1);

        player->name[MAX_PLAYER_NAME - 1] = '\0';
    }
    else
    {
        player->name[0] = '\0';
    }
}


/*=========================================================
    Deck
=========================================================*/

void add_deck_to_player(
    Player *player,
    const Deck *deck)
{
    /*
        Copy the deck structure.

        This copies the Card pointers, NOT the Card objects.
    */
    player->deck = *deck;

    /*
        Start drawing from the beginning.
    */
    player->deck_position = 0;
}


/*=========================================================
    Drawing
=========================================================*/

int draw_card_to_hand(Player *player)
{
    int i;

    /* Find first free hand slot */
    for (i = 0; i < MAX_HAND_SIZE; i++)
    {
        if (player->hand[i] == NULL)
            break;
    }

    /* Hand is full */
    if (i == MAX_HAND_SIZE)
        return 0;

    /* No cards left in deck */
    if (player->deck_position >= DECK_SIZE)
        return 0;

    /* Draw card */
    player->hand[i] =
        player->deck.cards[player->deck_position];

    player->deck_position++;

    /*
        Draw it on the corresponding board hand slot.
        If your player hand uses hand_slots[] from board_art:
    */
    draw_card(
        hand_slots[i].x,
        hand_slots[i].y,
        player->hand[i]);

    return 1;
}


int draw_cards_to_hand(
    Player *player,
    int amount)
{
    int i;
    int drawn = 0;

    for (i = 0; i < amount; i++)
    {
        if (!draw_card_to_hand(player))
            break;

        drawn++;
    }

    return drawn;
}