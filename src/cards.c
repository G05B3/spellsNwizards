#include "cards.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "cJSON.h"

Color CARD_TYPE_COLORS[CARD_TYPE_COUNT] = {
    [CARD_SPELL]       = LIGHT_CYAN,
    [CARD_CREATURE]    = LIGHT_YELLOW,
    [CARD_ARTIFACT]    = LIGHT_GREEN,
    [CARD_ENVIRONMENT] = WHITE   /* unused/default */
};

Color ATK_TYPE_COLORS[CARD_TYPE_COUNT] = {
    [ATTACK_NONE]       = WHITE,
    [ATTACK_PHYSICAL]    = RED,
    [ATTACK_MAGIC]    = MAGENTA,
    [ATTACK_VOID] = LIGHT_BLUE
};

/*=========================================================
    Helpers
=========================================================*/

static void init_card(Card *card)
{
    int i;

    card->id = -1;
    card->image_id = -1;

    card->name[0] = '\0';

    /* Standard parameters */
    card->cost = 0;
    card->atk = 0;
    card->hp = 0;

    /* Boolean abilities */
    card->first_strike = false;
    card->guardian = false;
    card->healer = false;
    card->vampirism = false;
    card->mana_harvest = false;

    /* Parameterized abilities */
    card->multiple_attacks = 1;
    card->mana_generation = 0;
    card->berserk = 0;

    for (i = 0; i < STATUS_COUNT; i++)
        card->inflicts[i] = 0;

    /* String abilities */
    card->call_kin[0] = '\0';
    card->army[0] = '\0';

    /* Immunities */
    for (i = 0; i < STATUS_COUNT; i++)
        card->status_immunity[i] = false;

    for (i = 0; i < ATTACK_TYPE_COUNT; i++)
        card->attack_immunity[i] = false;

    /* Races */
    for (i = 0; i < RACE_COUNT; i++)
        card->race[i] = false;

    /* Defaults */
    card->card_type = CARD_SPELL;
    card->atk_type = ATTACK_NONE;

    /* Palette defaults to white */
    for (i = 0; i < PALETTE_SIZE; i++)
        card->palette[i] = WHITE;
}


/*=========================================================
    Enum Parsing
=========================================================*/

static int parse_card_type(const char *name, CardType *type)
{
    if (!strcasecmp(name, "spell"))
    {
        *type = CARD_SPELL;
        return 1;
    }

    if (!strcasecmp(name, "creature"))
    {
        *type = CARD_CREATURE;
        return 1;
    }

    if (!strcasecmp(name, "artifact"))
    {
        *type = CARD_ARTIFACT;
        return 1;
    }

    if (!strcasecmp(name, "environment"))
    {
        *type = CARD_ENVIRONMENT;
        return 1;
    }

    return 0;
}


/*-------------------------------------------------------*/

static int parse_attack_type(const char *name, AttackType *type)
{
    if (!strcasecmp(name, "none"))
    {
        *type = ATTACK_NONE;
        return 1;
    }

    if (!strcasecmp(name, "physical") ||
        !strcasecmp(name, "phys"))
    {
        *type = ATTACK_PHYSICAL;
        return 1;
    }

    if (!strcasecmp(name, "magic") ||
        !strcasecmp(name, "mag"))
    {
        *type = ATTACK_MAGIC;
        return 1;
    }

    if (!strcasecmp(name, "void"))
    {
        *type = ATTACK_VOID;
        return 1;
    }

    return 0;
}


/*-------------------------------------------------------*/

static int parse_status_type(
    const char *name,
    StatusEffect *status)
{
    if (!strcasecmp(name, "burn"))
    {
        *status = STATUS_BURN;
        return 1;
    }

    if (!strcasecmp(name, "poison"))
    {
        *status = STATUS_POISON;
        return 1;
    }

    if (!strcasecmp(name, "freeze"))
    {
        *status = STATUS_FREEZE;
        return 1;
    }

    if (!strcasecmp(name, "flinch"))
    {
        *status = STATUS_FLINCH;
        return 1;
    }

    if (!strcasecmp(name, "invincibility"))
    {
        *status = STATUS_INVINCIBILITY;
        return 1;
    }

    if (!strcasecmp(name, "blight"))
    {
        *status = STATUS_BLIGHT;
        return 1;
    }

    if (!strcasecmp(name, "regeneration"))
    {
        *status = STATUS_REGENERATION;
        return 1;
    }

    if (!strcasecmp(name, "parry"))
    {
        *status = STATUS_PARRY;
        return 1;
    }

    return 0;
}


/*-------------------------------------------------------*/

static int parse_race(
    const char *name,
    RaceType *race)
{
    if (!strcasecmp(name, "human"))
    {
        *race = RACE_HUMAN;
        return 1;
    }

    if (!strcasecmp(name, "goblin"))
    {
        *race = RACE_GOBLIN;
        return 1;
    }

    if (!strcasecmp(name, "undead"))
    {
        *race = RACE_UNDEAD;
        return 1;
    }

    if (!strcasecmp(name, "demon"))
    {
        *race = RACE_DEMON;
        return 1;
    }

    if (!strcasecmp(name, "elf"))
    {
        *race = RACE_ELF;
        return 1;
    }

    if (!strcasecmp(name, "sea_creature") ||
        !strcasecmp(name, "sea creature"))
    {
        *race = RACE_SEA_CREATURE;
        return 1;
    }

    if (!strcasecmp(name, "dragon"))
    {
        *race = RACE_DRAGON;
        return 1;
    }

    if (!strcasecmp(name, "mythical_creature") ||
        !strcasecmp(name, "mythical creature"))
    {
        *race = RACE_MYTHICAL_CREATURE;
        return 1;
    }

    return 0;
}


/*=========================================================
    Simple JSON Helpers
=========================================================*/

static void parse_int_field(
    cJSON *json,
    const char *field,
    int *destination)
{
    cJSON *item =
        cJSON_GetObjectItem(json, field);

    if (cJSON_IsNumber(item))
        *destination = item->valueint;
}



/*=========================================================
    Ability Parsing
=========================================================*/

static void parse_ability(
    Card *card,
    cJSON *ability)
{
    cJSON *type =
        cJSON_GetObjectItem(
            ability,
            "type");

    cJSON *value =
        cJSON_GetObjectItem(
            ability,
            "value");

    if (!cJSON_IsString(type))
        return;


    /*-----------------------------------------------------
        Boolean abilities
    -----------------------------------------------------*/

    if (!strcasecmp(type->valuestring, "Healer"))
    {
        card->healer = true;
        return;
    }

    if (!strcasecmp(type->valuestring, "Vampirism"))
    {
        card->vampirism = true;
        return;
    }

    if (!strcasecmp(type->valuestring, "First Strike"))
    {
        card->first_strike = true;
        return;
    }

    if (!strcasecmp(type->valuestring, "Guardian"))
    {
        card->guardian = true;
        return;
    }

    if (!strcasecmp(type->valuestring, "Mana Harvest"))
    {
        card->mana_harvest = true;
        return;
    }


    /*-----------------------------------------------------
        Integer abilities
    -----------------------------------------------------*/

    if (!strcasecmp(type->valuestring, "Multiple Attacks"))
    {
        if (cJSON_IsNumber(value))
            card->multiple_attacks =
                value->valueint;

        return;
    }

    if (!strcasecmp(type->valuestring, "Mana Generation"))
    {
        if (cJSON_IsNumber(value))
            card->mana_generation =
                value->valueint;

        return;
    }

    if (!strcasecmp(type->valuestring, "Berserk"))
    {
        if (cJSON_IsNumber(value))
            card->berserk =
                value->valueint;

        return;
    }

    if (!strcasecmp(type->valuestring, "Poison Tip"))
    {
        if (cJSON_IsNumber(value))
            card->inflicts[STATUS_POISON] = value->valueint;
        return;
    }

    if (!strcasecmp(type->valuestring, "Flammable"))
    {
        if (cJSON_IsNumber(value))
            card->inflicts[STATUS_BURN] = value->valueint;
        return;
    }

    if (!strcasecmp(type->valuestring, "Cold Touch"))
    {
        if (cJSON_IsNumber(value))
            card->inflicts[STATUS_FREEZE] = value->valueint;
        return;
    }

    if (!strcasecmp(type->valuestring, "Blight Touch"))
    {
        if (cJSON_IsNumber(value))
            card->inflicts[STATUS_BLIGHT] = value->valueint;
        return;
    }

    /*-----------------------------------------------------
        String abilities
    -----------------------------------------------------*/

    if (!strcasecmp(type->valuestring, "Call Kin"))
    {
        if (cJSON_IsString(value))
        {
            snprintf(
                card->call_kin,
                sizeof(card->call_kin),
                "%s",
                value->valuestring);
        }

        return;
    }

    if (!strcasecmp(type->valuestring, "Army"))
    {
        if (cJSON_IsString(value))
        {
            snprintf(
                card->army,
                sizeof(card->army),
                "%s",
                value->valuestring);
        }

        return;
    }


    /*-----------------------------------------------------
        Attack Immunity
    -----------------------------------------------------*/

    if (!strcasecmp(type->valuestring, "Attack Immunity"))
    {
        if (cJSON_IsArray(value))
        {
            int count =
                cJSON_GetArraySize(value);

            for (int i = 0; i < count; i++)
            {
                cJSON *entry =
                    cJSON_GetArrayItem(
                        value,
                        i);

                if (!cJSON_IsString(entry))
                    continue;

                AttackType attack;

                if (parse_attack_type(
                        entry->valuestring,
                        &attack))
                {
                    card->attack_immunity[attack] = true;
                }
            }
        }

        return;
    }


    /*-----------------------------------------------------
        Status Immunity
    -----------------------------------------------------*/

    if (!strcasecmp(type->valuestring, "Status Immunity"))
    {
        if (cJSON_IsArray(value))
        {
            int count =
                cJSON_GetArraySize(value);

            for (int i = 0; i < count; i++)
            {
                cJSON *entry =
                    cJSON_GetArrayItem(
                        value,
                        i);

                if (!cJSON_IsString(entry))
                    continue;

                StatusEffect status;

                if (parse_status_type(
                        entry->valuestring,
                        &status))
                {
                    card->status_immunity[status] = true;
                }
            }
        }

        return;
    }
}


/*=========================================================
    Ability Array
=========================================================*/

static void parse_abilities(
    cJSON *json,
    Card *card)
{
    cJSON *abilities =
        cJSON_GetObjectItem(
            json,
            "abilities");

    if (!cJSON_IsArray(abilities))
        return;

    int count =
        cJSON_GetArraySize(abilities);

    for (int i = 0; i < count; i++)
    {
        cJSON *ability =
            cJSON_GetArrayItem(
                abilities,
                i);

        if (!cJSON_IsObject(ability))
            continue;

        parse_ability(
            card,
            ability);
    }
}


/*=========================================================
    Race Array
=========================================================*/

static void parse_races(
    cJSON *json,
    Card *card)
{
    cJSON *races =
        cJSON_GetObjectItem(
            json,
            "race");

    if (!cJSON_IsArray(races))
        return;

    int count =
        cJSON_GetArraySize(races);

    for (int i = 0; i < count; i++)
    {
        cJSON *race =
            cJSON_GetArrayItem(
                races,
                i);

        if (!cJSON_IsString(race))
            continue;

        RaceType race_type;

        if (parse_race(
                race->valuestring,
                &race_type))
        {
            card->race[race_type] = true;
        }
    }
}


/*=========================================================
    Palette
=========================================================*/

static void parse_palette(
    cJSON *json,
    Card *card)
{
    cJSON *palette =
        cJSON_GetObjectItem(
            json,
            "color_palette");

    if (!cJSON_IsArray(palette))
        return;

    int count =
        cJSON_GetArraySize(palette);

    if (count > PALETTE_SIZE)
        count = PALETTE_SIZE;

    for (int i = 0; i < count; i++)
    {
        cJSON *color =
            cJSON_GetArrayItem(
                palette,
                i);

        if (!cJSON_IsString(color))
            continue;

        int c =
            color_from_name(
                color->valuestring);

        if (c >= 0)
            card->palette[i] = (Color)c;
    }
}


/*=========================================================
    Card Parsing
=========================================================*/

static int parse_card(
    cJSON *json,
    Card *card)
{
    cJSON *name;
    cJSON *image;

    init_card(card);

    name =
        cJSON_GetObjectItem(
            json,
            "name");

    image =
        cJSON_GetObjectItem(
            json,
            "image_id");

    /*
        Name is the only mandatory field.
    */

    if (!cJSON_IsString(name))
        return 0;

    snprintf(
        card->name,
        sizeof(card->name),
        "%s",
        name->valuestring);


    /*-----------------------------------------------------
        Image
    -----------------------------------------------------*/

    if (cJSON_IsNumber(image))
        card->image_id =
            image->valueint;


    /*-----------------------------------------------------
        Standard parameters
    -----------------------------------------------------*/

    parse_int_field(
        json,
        "cost",
        &card->cost);

    parse_int_field(
        json,
        "atk",
        &card->atk);

    parse_int_field(
        json,
        "hp",
        &card->hp);


    /*-----------------------------------------------------
        Card type
    -----------------------------------------------------*/

    cJSON *card_type =
        cJSON_GetObjectItem(
            json,
            "card_type");

    if (cJSON_IsString(card_type))
    {
        parse_card_type(
            card_type->valuestring,
            &card->card_type);
    }


    /*-----------------------------------------------------
        Attack type
    -----------------------------------------------------*/

    cJSON *atk_type =
        cJSON_GetObjectItem(
            json,
            "atk_type");

    if (cJSON_IsString(atk_type))
    {
        parse_attack_type(
            atk_type->valuestring,
            &card->atk_type);
    }


    /*-----------------------------------------------------
        Palette
    -----------------------------------------------------*/

    parse_palette(
        json,
        card);


    /*-----------------------------------------------------
        Abilities
    -----------------------------------------------------*/

    parse_abilities(
        json,
        card);


    /*-----------------------------------------------------
        Races
    -----------------------------------------------------*/

    parse_races(
        json,
        card);


    /*
        on_cast / on_death are intentionally NOT
        represented in Card yet.

        We can add them once we decide what those
        numbers actually represent.
    */

    return 1;
}


/*=========================================================
    Public Functions
=========================================================*/

int load_cards(
    CardDatabase *db,
    const char *filename)
{
    FILE *f;

    long size;

    char *buffer;

    cJSON *root;
    cJSON *array;

    int i;

    memset(
        db,
        0,
        sizeof(*db));

    db->card_count = 0;

    f = fopen(
        filename,
        "rb");

    if (!f)
        return 0;

    fseek(
        f,
        0,
        SEEK_END);

    size = ftell(f);

    rewind(f);

    buffer = malloc(
        size + 1);

    if (!buffer)
    {
        fclose(f);
        return 0;
    }

    fread(
        buffer,
        1,
        size,
        f);

    buffer[size] = '\0';

    fclose(f);

    root =
        cJSON_Parse(buffer);

    free(buffer);

    if (!root)
        return 0;

    array =
        cJSON_GetObjectItem(
            root,
            "cards");

    if (!cJSON_IsArray(array))
    {
        cJSON_Delete(root);
        return 0;
    }

    for (i = 0;
         i < cJSON_GetArraySize(array) &&
         db->card_count < MAX_CARDS;
         i++)
    {
        cJSON *entry =
            cJSON_GetArrayItem(
                array,
                i);

        if (!cJSON_IsObject(entry))
            continue;

        if (!parse_card(
                entry,
                &db->cards[db->card_count]))
        {
            continue;
        }

        /*
            ID is assigned according to the
            position in the loaded database.
        */

        db->cards[db->card_count].id =
            db->card_count;

        db->card_count++;
    }

    cJSON_Delete(root);

    return 1;
}


/*-------------------------------------------------------*/

Card *get_card(
    CardDatabase *db,
    int id)
{
    if (id < 0 ||
        id >= db->card_count)
    {
        return NULL;
    }

    return &db->cards[id];
}

/*=========================================================
    Statistics
=========================================================*/

void print_statistics(const CardDatabase *db)
{
    int i;

    int card_type_count[CARD_TYPE_COUNT] = {0};
    int race_count[RACE_COUNT] = {0};

    int first_strike_count = 0;
    int guardian_count = 0;
    int healer_count = 0;
    int vampirism_count = 0;
    int mana_harvest_count = 0;

    int multiple_attacks_count = 0;
    int mana_generation_count = 0;
    int berserk_count = 0;

    int inflicts_count[STATUS_COUNT] = {0};

    int call_kin_count = 0;
    int army_count = 0;

    int attack_immunity_count[ATTACK_TYPE_COUNT] = {0};
    int status_immunity_count[STATUS_COUNT] = {0};

    int min_cost = 0;
    int max_cost = 0;

    int min_atk = 0;
    int max_atk = 0;

    int min_hp = 0;
    int max_hp = 0;

    int creature_count = 0;

    int min_cost_initialized = 0;
    int min_atk_initialized = 0;
    int min_hp_initialized = 0;

    /*
        Count basic statistics.
    */

    for (i = 0; i < db->card_count; i++)
    {
        const Card *card = &db->cards[i];

        /*-----------------------------------------
            Card type
        -----------------------------------------*/

        if (card->card_type >= 0 &&
            card->card_type < CARD_TYPE_COUNT)
        {
            card_type_count[card->card_type]++;
        }

        /*-----------------------------------------
            Races
        -----------------------------------------*/

        for (int r = 0; r < RACE_COUNT; r++)
        {
            if (card->race[r])
                race_count[r]++;
        }

        /*-----------------------------------------
            Boolean abilities
        -----------------------------------------*/

        if (card->first_strike)
            first_strike_count++;

        if (card->guardian)
            guardian_count++;

        if (card->healer)
            healer_count++;

        if (card->vampirism)
            vampirism_count++;

        if (card->mana_harvest)
            mana_harvest_count++;

        /*-----------------------------------------
            Parameterized abilities
        -----------------------------------------*/

        if (card->multiple_attacks > 1)
            multiple_attacks_count++;

        if (card->mana_generation > 0)
            mana_generation_count++;

        if (card->berserk > 0)
            berserk_count++;

        for (int st = 0; st < STATUS_COUNT; st++)
        {
            if (card->inflicts[st] > 0)
                inflicts_count[st]++;
        }

        /*-----------------------------------------
            String abilities
        -----------------------------------------*/

        if (card->call_kin[0] != '\0')
            call_kin_count++;

        if (card->army[0] != '\0')
            army_count++;

        /*-----------------------------------------
            Attack immunities
        -----------------------------------------*/

        for (int a = 0; a < ATTACK_TYPE_COUNT; a++)
        {
            if (card->attack_immunity[a])
                attack_immunity_count[a]++;
        }

        /*-----------------------------------------
            Status immunities
        -----------------------------------------*/

        for (int s = 0; s < STATUS_COUNT; s++)
        {
            if (card->status_immunity[s])
                status_immunity_count[s]++;
        }

        /*-----------------------------------------
            On-cast / on-death
        -----------------------------------------*/

        /*
            These are currently represented by the
            actual value. Since 0 is also potentially
            meaningful, this assumes that "no effect"
            is represented by 0.
        */

        /*
        if (card->on_cast != 0)
            on_cast_count++;

        if (card->on_death != 0)
            on_death_count++;
        */

        /*-----------------------------------------
            Cost
        -----------------------------------------*/

        if (!min_cost_initialized ||
            card->cost < min_cost)
        {
            min_cost = card->cost;
            min_cost_initialized = 1;
        }

        if (card->cost > max_cost)
            max_cost = card->cost;

        /*-----------------------------------------
            Creature combat statistics
        -----------------------------------------*/

        if (card->card_type == CARD_CREATURE)
        {
            creature_count++;

            if (!min_atk_initialized ||
                card->atk < min_atk)
            {
                min_atk = card->atk;
                min_atk_initialized = 1;
            }

            if (card->atk > max_atk)
                max_atk = card->atk;

            if (!min_hp_initialized ||
                card->hp < min_hp)
            {
                min_hp = card->hp;
                min_hp_initialized = 1;
            }

            if (card->hp > max_hp)
                max_hp = card->hp;
        }
    }

        /*=====================================================
        Print
    =====================================================*/

    printf("\n");
    printf("============================================================\n");
    printf("                       CARD STATISTICS\n");
    printf("============================================================\n");

    /*
        Two-column layout.
        Each column is 30 characters wide.
    */

    printf("\n");
    printf("%-32s%-32s\n",
           "GENERAL",
           "MANA COST");
    printf("------------------------------------------------------------\n");

    printf("%-25s %4d    %-25s %4d\n",
           "Total cards:", db->card_count,
           "Lowest cost:",
           db->card_count > 0 ? min_cost : 0);

    printf("%-25s %4d    %-25s %4d\n",
           "Creatures:",
           card_type_count[CARD_CREATURE],
           "Highest cost:",
           db->card_count > 0 ? max_cost : 0);

    printf("%-25s %4d\n",
           "Spells:",
           card_type_count[CARD_SPELL]);

    printf("%-25s %4d\n",
           "Artifacts:",
           card_type_count[CARD_ARTIFACT]);

    printf("%-25s %4d\n",
           "Environments:",
           card_type_count[CARD_ENVIRONMENT]);


    /*=====================================================
        Creature Combat / Boolean Abilities
    =====================================================*/

    printf("\n");
    printf("%-32s%-32s\n",
           "CREATURE COMBAT",
           "BOOLEAN ABILITIES");
    printf("------------------------------------------------------------\n");

    printf("%-25s %4d    %-25s %4d\n",
           "Creatures:",
           creature_count,
           "First Strike:",
           first_strike_count);

    if (creature_count > 0)
    {
        printf("%-25s %4d    %-25s %4d\n",
               "Lowest attack:",
               min_atk,
               "Guardian:",
               guardian_count);

        printf("%-25s %4d    %-25s %4d\n",
               "Highest attack:",
               max_atk,
               "Healer:",
               healer_count);

        printf("%-25s %4d    %-25s %4d\n",
               "Lowest HP:",
               min_hp,
               "Vampirism:",
               vampirism_count);

        printf("%-25s %4d    %-25s %4d\n",
               "Highest HP:",
               max_hp,
               "Mana Harvest:",
               mana_harvest_count);
    }
    else
    {
        printf("%-25s %4s    %-25s %4d\n",
               "Lowest attack:",
               "-",
               "Guardian:",
               guardian_count);

        printf("%-25s %4s    %-25s %4d\n",
               "Highest attack:",
               "-",
               "Healer:",
               healer_count);

        printf("%-25s %4s    %-25s %4d\n",
               "Lowest HP:",
               "-",
               "Vampirism:",
               vampirism_count);

        printf("%-25s %4s    %-25s %4d\n",
               "Highest HP:",
               "-",
               "Mana Harvest:",
               mana_harvest_count);
    }


    /*=====================================================
        Parameterized Abilities / Attack Immunities
    =====================================================*/

    printf("\n");
    printf("%-32s%-32s\n",
           "PARAMETERIZED ABILITIES",
           "ATTACK IMMUNITIES");
    printf("------------------------------------------------------------\n");

    printf("%-25s %4d    %-25s %4d\n",
           "Multiple Attacks:",
           multiple_attacks_count,
           "None:",
           attack_immunity_count[ATTACK_NONE]);

    printf("%-25s %4d    %-25s %4d\n",
           "Mana Generation:",
           mana_generation_count,
           "Physical:",
           attack_immunity_count[ATTACK_PHYSICAL]);

    printf("%-25s %4d    %-25s %4d\n",
           "Berserk:",
           berserk_count,
           "Magic:",
           attack_immunity_count[ATTACK_MAGIC]);

    printf("%-25s %4d    %-25s %4d\n",
           "Poison Tip:",
           inflicts_count[STATUS_POISON],
           "Void:",
           attack_immunity_count[ATTACK_VOID]);

    printf("%-25s %4d\n",
           "Flammable:",
           inflicts_count[STATUS_BURN]);

    printf("%-25s %4d\n",
           "Cold Touch:",
           inflicts_count[STATUS_FREEZE]);

    printf("%-25s %4d\n",
           "Blight Touch:",
           inflicts_count[STATUS_BLIGHT]);

    printf("%-25s %4d\n",
           "Call Kin:",
           call_kin_count);

    printf("%-25s %4d\n",
           "Army:",
           army_count);


    /*=====================================================
        Status Immunities / Races
    =====================================================*/

    printf("\n");
    printf("%-32s%-32s\n",
           "STATUS IMMUNITIES",
           "RACES");
    printf("------------------------------------------------------------\n");

    printf("%-25s %4d    %-25s %4d\n",
           "Burn:",
           status_immunity_count[STATUS_BURN],
           "Human:",
           race_count[RACE_HUMAN]);

    printf("%-25s %4d    %-25s %4d\n",
           "Poison:",
           status_immunity_count[STATUS_POISON],
           "Goblin:",
           race_count[RACE_GOBLIN]);

    printf("%-25s %4d    %-25s %4d\n",
           "Freeze:",
           status_immunity_count[STATUS_FREEZE],
           "Undead:",
           race_count[RACE_UNDEAD]);

    printf("%-25s %4d    %-25s %4d\n",
           "Flinch:",
           status_immunity_count[STATUS_FLINCH],
           "Demon:",
           race_count[RACE_DEMON]);

    printf("%-25s %4d    %-25s %4d\n",
           "Invincibility:",
           status_immunity_count[STATUS_INVINCIBILITY],
           "Elf:",
           race_count[RACE_ELF]);

    printf("%-25s %4d    %-25s %4d\n",
           "Blight:",
           status_immunity_count[STATUS_BLIGHT],
           "Sea Creature:",
           race_count[RACE_SEA_CREATURE]);

    printf("%-25s %4d    %-25s %4d\n",
           "Regeneration:",
           status_immunity_count[STATUS_REGENERATION],
           "Dragon:",
           race_count[RACE_DRAGON]);

    printf("%-25s %4d    %-25s %4d\n",
           "Parry:",
           status_immunity_count[STATUS_PARRY],
           "Mythical Creature:",
           race_count[RACE_MYTHICAL_CREATURE]);


    printf("\n");
    printf("============================================================\n");
}