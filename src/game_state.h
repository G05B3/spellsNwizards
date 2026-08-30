#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "player.h"
#include "board_state.h"

/*=========================================================
    Constants
=========================================================*/

#define PLAYER_COUNT 2

#define PLAYER_ONE 0
#define PLAYER_TWO 1


/*=========================================================
    Game State
=========================================================*/

typedef struct
{
    /*
        Players.

        For now this is simply two local players.

        Later:

            players[0] = local player
            players[1] = remote player

        or:

            players[0] = local player
            players[1] = AI
    */
    Player players[PLAYER_COUNT];


    /*
        Shared battlefield and graveyard.
    */
    BoardState board;

} GameState;


/*=========================================================
    Initialization
=========================================================*/

/*
    Initialize a new game state.

    Both players are initialized with empty decks/hands.
    The board and graveyard are empty.
*/
void init_game_state(
    GameState *game);


/*
    Initialize a game state with player names.
*/
void init_game_state_with_players(
    GameState *game,
    const char *player_one_name,
    const char *player_two_name);

/*
    Add/copy a player into the specified player slot.
*/
void add_player_to_game(
    GameState *game,
    int player_index,
    const Player *player);

/*=========================================================
    Casting
=========================================================*/

/*
    Allows the player to select a card from their hand
    and cast it.

    Returns:
        1 = card successfully cast
        0 = no card selected / unable to cast
*/
int cast_card(
    GameState *game,
    int player_index);


#endif