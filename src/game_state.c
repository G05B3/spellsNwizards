#include "selection.h"
#include "card_artwork.h"
#include "board_art.h"
#include "game_state.h"
#include "string.h"

/*=========================================================
    Helpers
=========================================================*/

/*
    Returns the first free creature slot.

    Returns -1 if all creature slots are occupied.
*/
static int find_free_creature_slot(
    const BoardState *board,
    int player)
{
    int i;

    for (i = FIRST_CREATURE_SLOT;
         i < BOARD_SLOTS;
         i++)
    {
        if (!board->cards[player][i].active)
            return i;
    }

    return -1;
}

/*
    Returns whether the artifact slot is free.
*/
static int artifact_slot_free(
    const BoardState *board,
    int player)
{
    return !board->cards[player][ARTIFACT_SLOT].active;
}

void init_game_state(GameState *game)
{
    if (game == NULL)
        return;

    memset(
        game,
        0,
        sizeof(BoardState));
    init_player(&game->players[PLAYER_ONE], "");
    init_player(&game->players[PLAYER_TWO], "");

    init_board_state(&game->board);
}

void init_game_state_with_players(
    GameState *game,
    const char *player_one_name,
    const char *player_two_name)
{
    init_player(
        &game->players[PLAYER_ONE],
        player_one_name);

    init_player(
        &game->players[PLAYER_TWO],
        player_two_name);

    init_board_state(&game->board);
}

void add_player_to_game(
    GameState *game,
    int player_index,
    const Player *player)
{
    if (player_index < 0 ||
        player_index >= PLAYER_COUNT)
        return;

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

    Card *card;

    int board_slot;

    if (game == NULL)
        return 0;

    if (player_index < 0 ||
        player_index >= 2)
    {
        return 0;
    }

    player = &game->players[player_index];

    /*
        ---------------------------------------------------
        Build selection pool from occupied hand slots.
        ---------------------------------------------------

        This is important because the hand itself is sparse.

            [ Warrior ][ NULL ][ Mage ][ NULL ]

        The selector should only allow the actual cards
        to be selected.
        ---------------------------------------------------
    */

    for (int i = 0;
         i < MAX_HAND_SIZE;
         i++)
    {
        if (player->hand[i] == NULL)
            continue;

        options[option_count].position =
            hand_slots[i];

        /*
            Card's normal outline.
        */
        if (player->hand[i]->card_type == CARD_ENVIRONMENT || player->hand[i]->card_type == CARD_NONE)
            options[option_count].normal_color = player->hand[i]->palette[0];
        else
            options[option_count].normal_color = CARD_TYPE_COLORS[player->hand[i]->card_type];

        option_count++;
    }

    if (option_count == 0)
        return 0;

    /*
        ---------------------------------------------------
        Select card.
        ---------------------------------------------------
    */

    selected =
        select_target(
            options,
            option_count);

    if (selected < 0)
        return 0;

    /*
        ---------------------------------------------------
        We need to recover the actual hand slot.

        Since the selection pool only contains occupied
        slots, find the corresponding hand entry again.
        ---------------------------------------------------
    */

    {
        int found = 0;

        for (int i = 0;
             i < MAX_HAND_SIZE;
             i++)
        {
            if (player->hand[i] == NULL)
                continue;

            if (found == selected)
            {
                card = player->hand[i];

                /*
                    Remove it from the hand immediately.
                */
                player->hand[i] = NULL;

                break;
            }

            found++;
        }
    }

    if (card == NULL)
        return 0;

    /*
        ---------------------------------------------------
        Draw card onto the bottom reaction-stack position.
        ---------------------------------------------------
    */

    draw_card(
        stack_slots[STACK_SLOTS - 1].x,
        stack_slots[STACK_SLOTS - 1].y,
        card);

    /*
        ---------------------------------------------------
        SPELL
        ---------------------------------------------------

        For now there is no spell resolution.

        Simply put the card into the shared graveyard.
        ---------------------------------------------------
    */

    if (card->card_type == CARD_SPELL)
    {
        add_to_graveyard(
            &game->board,
            card);

        return 1;
    }

    /*
        ---------------------------------------------------
        ENVIRONMENT
        ---------------------------------------------------

        Environments always replace the existing one.
        There is no "if free" check.
        ---------------------------------------------------
    */

    if (card->card_type == CARD_ENVIRONMENT)
    {
        /*
            TODO:
            If the old environment is supposed to enter
            the graveyard when replaced, that will happen
            here once we establish the exact environment
            replacement rules.
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

    /*
        ---------------------------------------------------
        ARTIFACT
        ---------------------------------------------------
    */

    if (card->card_type == CARD_ARTIFACT)
    {
        if (!artifact_slot_free(
                &game->board,
                player_index))
        {
            /*
                No available slot.

                The card has already been removed from the
                hand, so put it back.

                This is temporary; eventually casting should
                check whether a card can be played BEFORE
                removing it from the hand.
            */

            /*
                Find first free hand slot.
            */
            for (int i = 0;
                 i < MAX_HAND_SIZE;
                 i++)
            {
                if (player->hand[i] == NULL)
                {
                    player->hand[i] = card;
                    break;
                }
            }

            return 0;
        }

        board_slot = ARTIFACT_SLOT;
    }

    /*
        ---------------------------------------------------
        CREATURE
        ---------------------------------------------------
    */

    else if (card->card_type == CARD_CREATURE)
    {
        board_slot =
            find_free_creature_slot(
                &game->board,
                player_index);

        if (board_slot < 0)
        {
            /*
                No creature slot.

                Restore the card to the hand for now.
            */

            for (int i = 0;
                 i < MAX_HAND_SIZE;
                 i++)
            {
                if (player->hand[i] == NULL)
                {
                    player->hand[i] = card;
                    break;
                }
            }

            return 0;
        }
    }

    /*
        ---------------------------------------------------
        Create runtime card instance.
        ---------------------------------------------------
    */

    init_card_instance(
        &game->board.cards[player_index][board_slot],
        card);

    /*
        ---------------------------------------------------
        Draw card on board.
        ---------------------------------------------------
    */

    draw_card(
        board_slots[player_index][board_slot].x,
        board_slots[player_index][board_slot].y,
        card);

    return 1;
}