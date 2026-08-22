#ifndef CARD_H
#define CARD_H

#include <stdbool.h>
#include "access.h"
#include "cJSON.h"

/*=========================================================
    Constants
=========================================================*/

#define MAX_CARDS 1024

#define CALL_ABILTIIES_LEN 128


/*=========================================================
    Card Structure
=========================================================*/

typedef enum
{
    CARD_SPELL = 0,
    CARD_CREATURE,
    CARD_ARTIFACT,
    CARD_ENVIRONMENT,
    CARD_NONE,
    CARD_TYPE_COUNT
} CardType;

extern Color CARD_TYPE_COLORS[CARD_TYPE_COUNT];
extern Color ATK_TYPE_COLORS[CARD_TYPE_COUNT];

typedef enum
{
    ATTACK_NONE = 0,
    ATTACK_PHYSICAL,
    ATTACK_MAGIC,
    ATTACK_VOID,
    ATTACK_TYPE_COUNT
} AttackType;

typedef enum
{
    STATUS_BURN,
    STATUS_POISON,
    STATUS_FREEZE,
    STATUS_FLINCH,
    STATUS_INVINCIBILITY,
    STATUS_BLIGHT,
    STATUS_REGENERATION,
    STATUS_PARRY,

    /* Add future statuses here. */

    STATUS_COUNT
} StatusEffect;

typedef enum
{
    RACE_HUMAN = 0,
    RACE_GOBLIN,
    RACE_UNDEAD,
    RACE_DEMON,
    RACE_ELF,
    RACE_SEA_CREATURE,
    RACE_DRAGON,
    RACE_MYTHICAL_CREATURE,
    RACE_COUNT
} RaceType;


typedef struct
{
    int id;
    char name[256];
    int image_id;

    /*** Standard Params */
    int cost;
    int atk;
    int hp;

    /*** Boolean Abilities */
    bool first_strike; // Creature attacks first. If it kills the enemy creature, it will not take damage in return
    bool guardian; // Creature has priority over other allies regarding enemy attack targets
    bool healer; // Creature's attack step consists in healing allies instead
    bool vampirism; // Damage dealt to enemy creature (excluding damage that would put the target below 0 HP) is retrieved as HP gain for this creature
    bool mana_harvest; // Generates 1 extra mana when killing a creature

    /*** Parameterized Abilities */
    int multiple_attacks; // Creature can attack X times per turn (default is 1, and it's 0 for creatures not meant to attack)
    int mana_generation; // Generates X mana per turn. Generated mana is retrieved upon the creature's death
    int berserk; // Gains +X Atk after killing an enemy creature

    int poison_tip; // Inflicts Poison when attacking
    int flammable; // Inflicts Burn when attacking
    int cold_touch; // Inflicts Freeze when attacking

    char call_kin[CALL_ABILTIIES_LEN]; // Every turn, summon a creature of a target race or name, if possible
    char army[CALL_ABILTIIES_LEN]; // When the creature enters, fill the board of copies of a creature of a target race or name (as many as possible)

    bool status_immunity[STATUS_COUNT]; // Array of statuses the creature is immunte to
    bool attack_immunity[ATTACK_TYPE_COUNT]; // Array of attack types the creature is immune to

    bool race[RACE_COUNT];

    CardType card_type;
    AttackType atk_type;

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

void print_statistics(const CardDatabase *db);

#endif