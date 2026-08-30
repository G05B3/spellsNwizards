#include "selection.h"
#include "card_artwork.h"
#include "board_art.h"
#include "game_state.h"

#include <stdio.h>
#include <string.h>


/*=========================================================
    Helpers
=========================================================*/

static int find_free_creature_slot(
    const BoardState *board,
    int player)
{
    for (int i = FIRST_CREATURE_SLOT;
         i < BOARD_SLOTS;
         i++)
    {
        if (!board->cards[player][i].active)
            return i;
    }

    return -1;
}


static int artifact_slot_free(
    const BoardState *board,
    int player)
{
    return !board->cards[player][ARTIFACT_SLOT].active;
}


/*=========================================================
    Initialization
=========================================================*/

void init_game_state(
    GameState *game)
{
    if (game == NULL)
        return;

    memset(
        game,
        0,
        sizeof(GameState));

    init_player(
        &game->players[PLAYER_ONE],
        "");

    init_player(
        &game->players[PLAYER_TWO],
        "");

    init_board_state(
        &game->board);
}


void init_game_state_with_players(
    GameState *game,
    const char *player_one_name,
    const char *player_two_name)
{
    if (game == NULL)
        return;

    init_player(
        &game->players[PLAYER_ONE],
        player_one_name);

    init_player(
        &game->players[PLAYER_TWO],
        player_two_name);

    init_board_state(
        &game->board);
}


void add_player_to_game(
    GameState *game,
    int player_index,
    const Player *player)
{
    if (game == NULL ||
        player == NULL ||
        player_index < 0 ||
        player_index >= PLAYER_COUNT)
    {
        return;
    }

    game->players[player_index] = *player;
}


/*=========================================================
    Casting
=========================================================*/

int cast_card(
    GameState *game,
    int player_index)
{
    Player *player;

    SelectionOption options[MAX_HAND_SIZE];

    int option_count = 0;
    int selected;

    int hand_index;
    Card *card;

    int board_slot;


    if (game == NULL)
        return 0;

    if (player_index < 0 ||
        player_index >= PLAYER_COUNT)
    {
        return 0;
    }


    player =
        &game->players[player_index];


    /*-----------------------------------------------------
        Build selection pool
    -----------------------------------------------------*/

    for (int i = 0;
         i < MAX_HAND_SIZE;
         i++)
    {
        if (player->hand[i] == NULL)
            continue;

        options[option_count].position =
            hand_slots[i];

        /*
            Normal outline depends on the card type.
        */
        if (player->hand[i]->card_type == CARD_ENVIRONMENT ||
            player->hand[i]->card_type == CARD_NONE)
        {
            options[option_count].normal_color =
                player->hand[i]->palette[0];
        }
        else
        {
            options[option_count].normal_color =
                CARD_TYPE_COLORS[
                    player->hand[i]->card_type];
        }

        /*
            Keep track of the actual hand slot.

            This is important because the hand is sparse.
        */
        options[option_count].index = i;

        option_count++;
    }

    /*-----------------------------------------------------
        Select card
    -----------------------------------------------------*/

    selected =
        select_target(
            options,
            option_count);


    /*
        Player requested phase interruption.
    */
    if (selected == SELECT_PASS)
        return SELECT_PASS;


    /*
        Selection cancelled.
    */
    if (selected == SELECT_CANCEL)
        return 0;


    /*
        Convert selection index to actual hand slot.
    */
    hand_index =
        options[selected].index;

    card =
        player->hand[hand_index];


    if (card == NULL)
        return 0;


    /*-----------------------------------------------------
        Mana check
    -----------------------------------------------------*/

    if (player->mana < card->cost)
    {
        char message[512];

        snprintf(
            message,
            sizeof(message),
            "You don't have enough mana to cast %s",
            card->name);

        write_event(message);

        return 0;
    }


    /*-----------------------------------------------------
        Determine whether there is room on the board
    -----------------------------------------------------*/

    if (card->card_type == CARD_ARTIFACT)
    {
        if (!artifact_slot_free(
                &game->board,
                player_index))
        {
            write_event(
                "No artifact slot available.");

            return 0;
        }
    }

    else if (card->card_type == CARD_CREATURE)
    {
        board_slot =
            find_free_creature_slot(
                &game->board,
                player_index);

        if (board_slot < 0)
        {
            write_event(
                "No creature slot available.");

            return 0;
        }
    }


    /*-----------------------------------------------------
        Pay mana
    -----------------------------------------------------*/

    player->mana -= card->cost;
    draw_players_info(
    game->players[0].name,
    game->players[0].hp,
    game->players[0].mana,
    game->players[1].name,
    game->players[1].hp,
    game->players[1].mana);


    /*-----------------------------------------------------
        Remove from hand
    -----------------------------------------------------*/

    player->hand[hand_index] = NULL;


    /*
        Restore the hand card's normal outline after
        selection, since the card will disappear.
    */
    /*draw_outline(
        hand_slots[hand_index].x,
        hand_slots[hand_index].y,
        options[selected].normal_color);*/
    delete_card(hand_slots[hand_index].x, hand_slots[hand_index].y);


    /*-----------------------------------------------------
        Reaction stack
    -----------------------------------------------------*/

    draw_card(
        stack_slots[STACK_SLOTS - 1].x,
        stack_slots[STACK_SLOTS - 1].y,
        card);


    /*-----------------------------------------------------
        Spell
    -----------------------------------------------------*/

    if (card->card_type == CARD_SPELL)
    {
        add_to_graveyard(
            &game->board,
            card);

        return 1;
    }


    /*-----------------------------------------------------
        Environment
    -----------------------------------------------------*/

    if (card->card_type == CARD_ENVIRONMENT)
    {
        /*
            Environment always overwrites the existing
            environment.
        */
        init_card_instance(
            &game->board.environment,
            card);

        draw_card(
            environment_slot.x,
            environment_slot.y,
            card);

        return 1;
    }


    /*-----------------------------------------------------
        Artifact
    -----------------------------------------------------*/

    if (card->card_type == CARD_ARTIFACT)
    {
        board_slot =
            ARTIFACT_SLOT;
    }


    /*-----------------------------------------------------
        Creature
    -----------------------------------------------------*/

    else if (card->card_type == CARD_CREATURE)
    {
        /*
            board_slot was already found above.
        */
    }


    /*-----------------------------------------------------
        Create card instance
    -----------------------------------------------------*/

    init_card_instance(
        &game->board.cards[player_index][board_slot],
        card);


    /*-----------------------------------------------------
        Draw card on board
    -----------------------------------------------------*/

    draw_card(
        board_slots[player_index][board_slot].x,
        board_slots[player_index][board_slot].y,
        card);


    return 1;
}


/*=========================================================
    Main Phase
=========================================================*/

int main_phase(
    GameState *game,
    int player_index)
{
    int result;

    if (game == NULL ||
        player_index < 0 ||
        player_index >= PLAYER_COUNT)
    {
        return 0;
    }


    /*
        Keep allowing the player to cast cards until
        they explicitly pass the phase.
    */
    while (1)
    {
        result =
            cast_card(
                game,
                player_index);


        /*
            P = pass main phase.
        */
        if (result == SELECT_PASS)
        {
            write_event(
                "Main phase passed.");

            break;
        }


        /*
            Card was successfully cast.

            Continue the main phase and allow another
            card to be selected.
        */
        if (result == 1)
            continue;


        /*
            Failed cast.

            This could be insufficient mana, no board
            space, etc.

            Stay in the main phase and let the player
            choose again.
        */
    }

    return 1;
}

int attack(
    GameState *game,
    int attacker_player,
    int attacker_slot,
    int target_player,
    int target_slot)
{
    CardInstance *attacker;
    CardInstance *target;

    if (game == NULL)
        return 0;

    if (attacker_player < 0 ||
        attacker_player >= PLAYER_COUNT ||
        target_player < 0 ||
        target_player >= PLAYER_COUNT)
    {
        return 0;
    }

    if (attacker_slot < FIRST_CREATURE_SLOT ||
        attacker_slot >= BOARD_SLOTS ||
        target_slot < FIRST_CREATURE_SLOT ||
        target_slot >= BOARD_SLOTS)
    {
        return 0;
    }

    attacker =
        &game->board.cards[attacker_player][attacker_slot];

    target =
        &game->board.cards[target_player][target_slot];

    /*
        Both positions must contain active creatures.
    */
    if (!attacker->active ||
        !target->active)
    {
        return 0;
    }

    if (attacker->card->card_type != CARD_CREATURE ||
        target->card->card_type != CARD_CREATURE)
    {
        return 0;
    }

    /*
        The attacker must still have an attack available.
    */
    if (attacker->attacks_remaining <= 0)
        return 0;

    /*
        Basic combat resolution.

        For now both creatures deal their ATK
        simultaneously.
    */
    target->hp -= attacker->atk;
    attacker->hp -= target->atk;

    /*
        Consume one attack.
    */
    attacker->attacks_remaining--;

    /*
        Redraw both cards using their runtime state.
    */
    draw_cardInstance(
        board_slots[attacker_player][attacker_slot].x,
        board_slots[attacker_player][attacker_slot].y,
        attacker);

    draw_cardInstance(
        board_slots[target_player][target_slot].x,
        board_slots[target_player][target_slot].y,
        target);

    return 1;
}