#include "board_state.h"

#include <string.h>


/*=========================================================*
    Card Instance
=========================================================*/

void init_card_instance(
    CardInstance *instance,
    Card *card)
{
    if (instance == NULL)
        return;

    /*
        Start from a completely clean instance.
    */
    memset(instance, 0, sizeof(CardInstance));

    instance->card = card;

    if (card == NULL)
        return;


    /*-----------------------------------------------------*
        Combat state
    *-----------------------------------------------------*/

    instance->atk = card->atk;
    instance->hp  = card->hp;


    /*-----------------------------------------------------*
        Boolean abilities
    *-----------------------------------------------------*/

    instance->first_strike = card->first_strike;
    instance->guardian     = card->guardian;
    instance->healer       = card->healer;
    instance->vampirism    = card->vampirism;
    instance->mana_harvest = card->mana_harvest;


    /*-----------------------------------------------------*
        Parameterized abilities
    *-----------------------------------------------------*/

    instance->multiple_attacks = card->multiple_attacks;
    instance->mana_generation  = card->mana_generation;
    instance->berserk          = card->berserk;


    /*-----------------------------------------------------*
        Status / attack immunities
    *-----------------------------------------------------*/

    for (int i = 0; i < STATUS_COUNT; i++)
    {
        instance->current_status[i] =
            0;

        instance->status_immunity[i] =
            card->status_immunity[i];
    }

    for (int i = 0; i < ATTACK_TYPE_COUNT; i++)
    {
        instance->attack_immunity[i] =
            card->attack_immunity[i];
    }


    instance->active = true;
}


/*=========================================================*
    Clear Instance
=========================================================*/

void clear_card_instance(
    CardInstance *instance)
{
    if (instance == NULL)
        return;

    memset(
        instance,
        0,
        sizeof(CardInstance));
}


/*=========================================================*
    Board State
=========================================================*/

void init_board_state(
    BoardState *board)
{
    if (board == NULL)
        return;

    memset(
        board,
        0,
        sizeof(BoardState));
}


/*=========================================================*
    Get Instance
=========================================================*/

CardInstance *get_board_instance(
    BoardState *board,
    int side,
    int slot)
{
    if (board == NULL)
        return NULL;

    if (side < 0 || side >= 2)
        return NULL;

    if (slot < 0 || slot >= BOARD_SLOTS)
        return NULL;

    if (!board->cards[side][slot].active)
        return NULL;

    return &board->cards[side][slot];
}


/*=========================================================*
    Place Instance
=========================================================*/

int place_card_instance(
    BoardState *board,
    int side,
    int slot,
    const CardInstance *instance)
{
    if (board == NULL || instance == NULL)
        return 0;

    if (side < 0 || side >= 2)
        return 0;

    if (slot < 0 || slot >= BOARD_SLOTS)
        return 0;

    /*
        Do not overwrite an occupied slot.
    */
    if (board->cards[side][slot].active)
        return 0;

    board->cards[side][slot] = *instance;

    board->cards[side][slot].active = true;

    return 1;
}


/*=========================================================*
    Remove Instance
=========================================================*/

int remove_card_instance(
    BoardState *board,
    int side,
    int slot,
    CardInstance *out_instance)
{
    if (board == NULL)
        return 0;

    if (side < 0 || side >= 2)
        return 0;

    if (slot < 0 || slot >= BOARD_SLOTS)
        return 0;

    if (!board->cards[side][slot].active)
        return 0;

    if (out_instance != NULL)
    {
        *out_instance =
            board->cards[side][slot];
    }

    clear_card_instance(
        &board->cards[side][slot]);

    return 1;
}

CardInstance *get_environment(
    BoardState *board)
{
    if (board == NULL)
        return NULL;

    if (!board->environment.active)
        return NULL;

    return &board->environment;
}

/*=========================================================*
    Graveyard
=========================================================*/

int add_to_graveyard(
    BoardState *board,
    Card *card)
{
    if (board == NULL || card == NULL)
        return 0;

    if (board->graveyard.count >= MAX_GRAVEYARD_SIZE)
        return 0;

    board->graveyard.cards[
        board->graveyard.count
    ] = card;

    board->graveyard.count++;

    return 1;
}


Card *pop_graveyard(
    BoardState *board)
{
    Card *card;

    if (board == NULL)
        return NULL;

    if (board->graveyard.count <= 0)
        return NULL;

    board->graveyard.count--;

    card =
        board->graveyard.cards[
            board->graveyard.count
        ];

    board->graveyard.cards[
        board->graveyard.count
    ] = NULL;

    return card;
}


Card *peek_graveyard(
    const BoardState *board)
{
    if (board == NULL)
        return NULL;

    if (board->graveyard.count <= 0)
        return NULL;

    return board->graveyard.cards[
        board->graveyard.count - 1
    ];
}