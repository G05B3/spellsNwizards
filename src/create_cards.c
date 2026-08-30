/*
    carddb.c
    --------
    Simple card database manager.

    Compile with:
        gcc carddb.c cJSON.c -o carddb
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include "cJSON.h"

#define CARD_INFO_FILE "src/card_info.json"

#define MAX_NAME_LEN 256
#define MAX_COLORS 32
#define MAX_COLOR_LEN 32

#define MAX_ABILITIES 32
#define MAX_ABILITY_VALUES 16
#define MAX_ABILITY_LEN 64
#define MAX_ABILITY_VALUE_LEN 256

typedef enum
{
    ABILITY_VALUE_NONE,
    ABILITY_VALUE_NUMBER,
    ABILITY_VALUE_STRING,
    ABILITY_VALUE_ARRAY

} AbilityValueType;

typedef struct
{
    char type[MAX_ABILITY_LEN];

    AbilityValueType value_type;

    int number;

    char string[MAX_ABILITY_VALUE_LEN];

    char values[MAX_ABILITY_VALUES][MAX_ABILITY_VALUE_LEN];
    int value_count;

} Ability;

typedef struct
{
    char name[MAX_NAME_LEN];

    int image_id;

    int palette_size;
    char palette[MAX_COLORS][MAX_COLOR_LEN];

} Card;

/*=========================================================
    Utility functions
=========================================================*/

static int file_exists(const char *filename)
{
    FILE *f = fopen(filename, "r");

    if (f)
    {
        fclose(f);
        return 1;
    }

    return 0;
}

/*-------------------------------------------------------*/

static char *read_entire_file(const char *filename)
{
    FILE *f = fopen(filename, "rb");

    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);

    long size = ftell(f);

    rewind(f);

    char *buffer = malloc(size + 1);

    if (!buffer)
    {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);

    buffer[size] = '\0';

    fclose(f);

    return buffer;
}

/*-------------------------------------------------------*/

static int write_json_file(cJSON *root)
{
    char *text = cJSON_Print(root);

    if (!text)
        return 0;

    FILE *f = fopen(CARD_INFO_FILE, "w");

    if (!f)
    {
        free(text);
        return 0;
    }

    fputs(text, f);

    fclose(f);

    free(text);

    return 1;
}

/*=========================================================
    JSON database
=========================================================*/

static cJSON *create_empty_database(void)
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddItemToObject(
        root,
        "cards",
        cJSON_CreateArray());

    return root;
}

/*-------------------------------------------------------*/

static cJSON *load_database(void)
{
    if (!file_exists(CARD_INFO_FILE))
        return create_empty_database();

    char *text = read_entire_file(CARD_INFO_FILE);

    if (!text)
        return create_empty_database();

    cJSON *root = cJSON_Parse(text);

    free(text);

    if (!root)
        return create_empty_database();

    cJSON *cards =
        cJSON_GetObjectItem(root, "cards");

    if (!cJSON_IsArray(cards))
    {
        cJSON_Delete(root);
        return create_empty_database();
    }

    return root;
}

/*=========================================================
    Validation
=========================================================*/

static int is_integer(const char *s)
{
    if (*s == '\0')
        return 0;

    if (*s == '-')
        s++;

    if (*s == '\0')
        return 0;

    while (*s)
    {
        if (!isdigit((unsigned char)*s))
            return 0;

        s++;
    }

    return 1;
}

/*-------------------------------------------------------*/

/*
    Very relaxed validation.

    Accepts things like:

        red
        dark_blue
        Blue
        orange-2

    Rejects spaces and symbols.
*/

static int valid_color(const char *s)
{
    static const char *valid_colors[] =
        {
            "black",
            "white",

            "red",
            "green",
            "blue",

            "yellow",
            "orange",
            "cyan",

            "purple",
            "pink",

            "gray",
            "dark_gray",

            "lime",
            "brown",
            "olive",
            "magenta",
            "light_yellow",
            "light_cyan",
            "light_green",
            "light_blue",
            "light_brown",

            "dark_green"
        };
            

    const int n =
        sizeof(valid_colors) /
        sizeof(valid_colors[0]);

    for (int i = 0; i < n; i++)
    {
        if (!strcmp(s, valid_colors[i]))
            return 1;
    }

    return 0;
}

static int valid_atk_type(const char *s)
{
    return !strcasecmp(s, "Physical") ||
           !strcasecmp(s, "Phys") ||
           !strcasecmp(s, "Magic") ||
           !strcasecmp(s, "Mag") ||
           !strcasecmp(s, "Void") ||
           !strcasecmp(s, "None");
}

static int valid_card_type(const char *s)
{
    return !strcasecmp(s, "creature") ||
           !strcasecmp(s, "spell") ||
           !strcasecmp(s, "artifact") ||
           !strcasecmp(s, "environment");
}

/*=========================================================
    Ability Helpers
=========================================================*/

/*
    Returns 1 if the ability is a valid ability name.
*/
static int valid_ability_name(const char *name)
{
    const char *abilities[] =
        {
            "Healer",
            "Vampirism",
            "First Strike",
            "Multiple Attacks",
            "Mana Harvest",
            "Mana Generation",
            "Guardian",
            "Berserk",
            "Attack Immunity",
            "Status Immunity",
            "Call Kin",
            "Army",

            /* Future abilities can be added here. */
            "Poison Tip",
            "Flammable",
            "Cold Touch"};

    int count =
        sizeof(abilities) / sizeof(abilities[0]);

    int i;

    for (i = 0; i < count; i++)
    {
        if (!strcasecmp(name, abilities[i]))
            return 1;
    }

    return 0;
}

/*-------------------------------------------------------*/

/*
    Determines what kind of value an ability expects.
*/
static AbilityValueType ability_value_type(
    const char *name)
{
    /*
        Abilities with no parameters.
    */

    if (!strcasecmp(name, "Healer") ||
        !strcasecmp(name, "Vampirism") ||
        !strcasecmp(name, "First Strike") ||
        !strcasecmp(name, "Mana Harvest") ||
        !strcasecmp(name, "Guardian"))
    {
        return ABILITY_VALUE_NONE;
    }

    /*
        Abilities with a single integer.
    */

    if (!strcasecmp(name, "Multiple Attacks") ||
        !strcasecmp(name, "Mana Generation") ||
        !strcasecmp(name, "Berserk") ||
        !strcasecmp(name, "Poison Tip") ||
        !strcasecmp(name, "Flammable") ||
        !strcasecmp(name, "Cold Touch"))
    {
        return ABILITY_VALUE_NUMBER;
    }

    /*
        Abilities with one or more strings.
    */

    if (!strcasecmp(name, "Attack Immunity") ||
        !strcasecmp(name, "Status Immunity"))
    {
        return ABILITY_VALUE_ARRAY;
    }

    /*
        Abilities with one string.
    */

    if (!strcasecmp(name, "Call Kin") ||
        !strcasecmp(name, "Army"))
    {
        return ABILITY_VALUE_STRING;
    }

    return ABILITY_VALUE_NONE;
}

static int parse_ability(
    const char *text,
    Ability *ability)
{
    char buffer[MAX_ABILITY_VALUE_LEN];

    char *token;

    char *tokens[MAX_ABILITY_VALUES];

    int token_count = 0;

    int i;

    AbilityValueType type;

    /*
        Make a writable copy because strtok()
        modifies the string.
    */

    snprintf(
        buffer,
        sizeof(buffer),
        "%s",
        text);

    /*
        Split the ability specification into words.
    */

    token = strtok(buffer, " ");

    while (token != NULL)
    {
        if (token_count >= MAX_ABILITY_VALUES)
        {
            printf(
                "Error: too many parameters for ability \"%s\".\n",
                text);

            return 0;
        }

        tokens[token_count++] = token;

        token = strtok(NULL, " ");
    }

    if (token_count == 0)
    {
        printf(
            "Error: empty ability.\n");

        return 0;
    }

    /*
        Find the ability name.

        Some ability names contain spaces, so
        determine the longest valid prefix.

        Example:

            "Berserk 2"

        becomes:

            name  = "Berserk"
            value = "2"

        while:

            "Status Immunity Burn Poison"

        becomes:

            name   = "Status Immunity"
            values = "Burn", "Poison"
    */

    char ability_name[MAX_ABILITY_LEN];

    int name_token_count = 0;

    int found = 0;

    /*
        Try increasingly long prefixes.

        Since all currently defined ability names
        contain at most two words, this is enough
        for now.
    */

    for (i = token_count; i >= 1; i--)
    {
        int j;

        ability_name[0] = '\0';

        for (j = 0; j < i; j++)
        {
            if (j > 0)
                strncat(
                    ability_name,
                    " ",
                    sizeof(ability_name) -
                        strlen(ability_name) - 1);

            strncat(
                ability_name,
                tokens[j],
                sizeof(ability_name) -
                    strlen(ability_name) - 1);
        }

        if (valid_ability_name(ability_name))
        {
            name_token_count = i;
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf(
            "Error: unknown ability \"%s\".\n",
            text);

        return 0;
    }

    /*
        Initialize the structure.
    */

    memset(
        ability,
        0,
        sizeof(*ability));

    snprintf(
        ability->type,
        sizeof(ability->type),
        "%s",
        ability_name);

    type = ability_value_type(
        ability_name);

    ability->value_type = type;

    /*
        Check the number/type of parameters.
    */

    switch (type)
    {
    /*---------------------------------------------*/
    case ABILITY_VALUE_NONE:

        if (token_count != name_token_count)
        {
            printf(
                "Error: ability \"%s\" "
                "does not take parameters.\n",
                ability_name);

            return 0;
        }

        break;

    /*---------------------------------------------*/
    case ABILITY_VALUE_NUMBER:
    {
        int value;

        if (token_count != name_token_count + 1)
        {
            printf(
                "Error: ability \"%s\" "
                "requires one number.\n",
                ability_name);

            return 0;
        }

        if (!is_integer(
                tokens[name_token_count]))
        {
            printf(
                "Error: ability \"%s\" "
                "requires a numeric parameter.\n",
                ability_name);

            return 0;
        }

        value = atoi(
            tokens[name_token_count]);

        ability->number = value;

        break;
    }

    /*---------------------------------------------*/
    case ABILITY_VALUE_STRING:
    {
        if (token_count <= name_token_count)
        {
            printf(
                "Error: ability \"%s\" "
                "requires a parameter.\n",
                ability_name);

            return 0;
        }

        ability->string[0] = '\0';

        for (i = name_token_count;
             i < token_count;
             i++)
        {
            if (i > name_token_count)
            {
                strncat(
                    ability->string,
                    " ",
                    sizeof(ability->string) -
                        strlen(ability->string) - 1);
            }

            strncat(
                ability->string,
                tokens[i],
                sizeof(ability->string) -
                    strlen(ability->string) - 1);
        }

        break;
    }

    /*---------------------------------------------*/
    case ABILITY_VALUE_ARRAY:
    {
        int value_count =
            token_count - name_token_count;

        if (value_count <= 0)
        {
            printf(
                "Error: ability \"%s\" "
                "requires at least one parameter.\n",
                ability_name);

            return 0;
        }

        if (value_count > MAX_ABILITY_VALUES)
        {
            printf(
                "Error: too many parameters "
                "for ability \"%s\".\n",
                ability_name);

            return 0;
        }

        ability->value_count =
            value_count;

        for (i = 0;
             i < value_count;
             i++)
        {
            snprintf(
                ability->values[i],
                sizeof(ability->values[i]),
                "%s",
                tokens[name_token_count + i]);
        }

        break;
    }

    /*---------------------------------------------*/
    default:

        printf(
            "Error: unsupported ability type.\n");

        return 0;
    }

    return 1;
}

/*=========================================================
    Parsed command line
=========================================================*/

typedef struct
{
    int add_card;

    char name[MAX_NAME_LEN];

    int image_id;
    int image_given;

    int palette_given;

    int palette_size;
    char palette[MAX_COLORS][MAX_COLOR_LEN];

    int atk;
    int hp;
    int cost;

    char atk_type[32];
    int atk_type_given;

    char card_type[32];
    int card_type_given;

    Ability abilities[MAX_ABILITIES];
    int ability_count;

    int on_cast;
    int on_cast_given;

    int on_death;
    int on_death_given;

    char race[MAX_COLORS][MAX_COLOR_LEN];
    int race_size;
    int race_given;

} ParsedArguments;

/*-------------------------------------------------------*/

static void init_arguments(ParsedArguments *args)
{
    memset(args, 0, sizeof(*args));

    args->image_id = -1;

    args->atk = 0;
    args->hp = 0;
    args->cost = 0;

    strcpy(args->atk_type, "None");
    strcpy(args->card_type, "spell");
}

/*=========================================================
    Argument parsing
=========================================================*/

static int parse_arguments(
    int argc,
    char **argv,
    ParsedArguments *args)
{
    init_arguments(args);

    int i = 1;

    while (i < argc)
    {
        /*------------------------------------------
            ADD CARD
        ------------------------------------------*/

        if (!strcmp(argv[i], "--add_card") ||
            !strcmp(argv[i], "-a"))
        {
            if (i + 1 >= argc)
            {
                printf("Error: missing card name.\n");
                return 0;
            }

            if (strlen(argv[i + 1]) == 0)
            {
                printf("Error: empty card name.\n");
                return 0;
            }

            args->add_card = 1;

            strncpy(
                args->name,
                argv[i + 1],
                MAX_NAME_LEN - 1);

            i += 2;

            continue;
        }

        /*------------------------------------------
            IMAGE ID
        ------------------------------------------*/

        if (!strcmp(argv[i], "--image_id") ||
            !strcmp(argv[i], "-i"))
        {
            if (i + 1 >= argc)
            {
                printf("Error: missing image id.\n");
                return 0;
            }

            if (!is_integer(argv[i + 1]))
            {
                printf("Error: invalid image id.\n");
                return 0;
            }

            args->image_given = 1;
            args->image_id = atoi(argv[i + 1]);

            i += 2;

            continue;
        }

        /*------------------------------------------
            PALETTE
        ------------------------------------------*/

        if (!strcmp(argv[i], "--palette") ||
            !strcmp(argv[i], "-p"))
        {
            args->palette_given = 1;

            i++;

            /* Require at least one color */
            if (i >= argc || argv[i][0] == '-')
            {
                printf("Error: palette requires at least one color.\n");
                return 0;
            }

            while (i < argc &&
                   argv[i][0] != '-')
            {
                if (!valid_color(argv[i]))
                {
                    printf("Error: invalid color \"%s\".\n",
                           argv[i]);
                    return 0;
                }

                if (args->palette_size >= MAX_COLORS)
                {
                    printf("Error: palette too large.\n");
                    return 0;
                }

                snprintf(args->palette[args->palette_size],
                         MAX_COLOR_LEN,
                         "%s",
                         argv[i]);

                args->palette_size++;

                i++;
            }

            continue;
        }

        /******
         * Atk
         */
        if (!strcmp(argv[i], "--atk"))
        {
            if (i + 1 >= argc)
            {
                printf("Error: missing attack value.\n");
                return 0;
            }

            if (!is_integer(argv[i + 1]))
            {
                printf("Error: invalid attack value.\n");
                return 0;
            }

            args->atk = atoi(argv[i + 1]);

            i += 2;
            continue;
        }

        /*****
         * HP
         */
        if (!strcmp(argv[i], "--hp"))
        {
            if (i + 1 >= argc)
            {
                printf("Error: missing hp value.\n");
                return 0;
            }

            if (!is_integer(argv[i + 1]))
            {
                printf("Error: invalid hp value.\n");
                return 0;
            }

            args->hp = atoi(argv[i + 1]);

            i += 2;
            continue;
        }

        /*****
         * Mana Cost
         */
        if (!strcmp(argv[i], "--cost"))
        {
            if (i + 1 >= argc)
            {
                printf("Error: missing mana cost value.\n");
                return 0;
            }

            if (!is_integer(argv[i + 1]))
            {
                printf("Error: invalid mana cost value.\n");
                return 0;
            }

            args->cost = atoi(argv[i + 1]);

            i += 2;
            continue;
        }

        if (!strcmp(argv[i], "--atk-type") || !strcmp(argv[i], "--atk_type") || !strcmp(argv[i], "-at"))
        {
            if (i + 1 >= argc)
            {
                printf("Error: missing attack type.\n");
                return 0;
            }

            if (!valid_atk_type(argv[i + 1]))
            {
                printf("Error: invalid attack type \"%s\".\n",
                       argv[i + 1]);
                return 0;
            }

            if (!strcasecmp(argv[i + 1], "Phys"))
                strcpy(args->atk_type, "Physical");
            else if (!strcasecmp(argv[i + 1], "Mag"))
                strcpy(args->atk_type, "Magic");
            else
                strcpy(args->atk_type, argv[i + 1]);

            args->atk_type_given = 1;

            i += 2;
            continue;
        }

        if (!strcmp(argv[i], "--card-type") || !strcmp(argv[i], "--card_type") || !strcmp(argv[i], "-ct"))
        {
            if (i + 1 >= argc)
            {
                printf("Error: missing card type.\n");
                return 0;
            }

            if (!valid_card_type(argv[i + 1]))
            {
                printf("Error: invalid card type \"%s\".\n",
                       argv[i + 1]);
                return 0;
            }

            if (!strcasecmp(argv[i + 1], "creature"))
                strcpy(args->card_type, "creature");
            else if (!strcasecmp(argv[i + 1], "spell"))
                strcpy(args->card_type, "spell");
            else if (!strcasecmp(argv[i + 1], "artifact"))
                strcpy(args->card_type, "artifact");
            else
                strcpy(args->card_type, "environment");

            args->card_type_given = 1;

            i += 2;
            continue;
        }

        if (!strcmp(argv[i], "--on_cast"))
        {
            if (i + 1 >= argc)
            {
                printf("Error: missing on_cast value.\n");
                return 0;
            }

            if (!is_integer(argv[i + 1]))
            {
                printf("Error: invalid on_cast value.\n");
                return 0;
            }

            args->on_cast = atoi(argv[i + 1]);
            args->on_cast_given = 1;

            i += 2;
            continue;
        }

        if (!strcmp(argv[i], "--on_death"))
        {
            if (i + 1 >= argc)
            {
                printf("Error: missing on_death value.\n");
                return 0;
            }

            if (!is_integer(argv[i + 1]))
            {
                printf("Error: invalid on_death value.\n");
                return 0;
            }

            args->on_death = atoi(argv[i + 1]);
            args->on_death_given = 1;

            i += 2;
            continue;
        }

        if (!strcmp(argv[i], "--abilities"))
        {
            i++;

            /*
                At least one ability must follow --abilities.
            */

            if (i >= argc || argv[i][0] == '-')
            {
                printf(
                    "Error: --abilities requires "
                    "at least one ability.\n");

                return 0;
            }

            while (i < argc &&
                   argv[i][0] != '-')
            {
                if (args->ability_count >= MAX_ABILITIES)
                {
                    printf(
                        "Error: too many abilities.\n");

                    return 0;
                }

                if (!parse_ability(
                        argv[i],
                        &args->abilities[args->ability_count]))
                {
                    return 0;
                }

                args->ability_count++;

                i++;
            }

            continue;
        }

        if (!strcmp(argv[i], "--race") ||
            !strcmp(argv[i], "-r"))
        {
            args->race_given = 1;

            i++;

            if (i >= argc || argv[i][0] == '-')
            {
                printf(
                    "Error: --race requires at least one race.\n");

                return 0;
            }

            while (i < argc &&
                   argv[i][0] != '-')
            {
                if (args->race_size >= MAX_COLORS)
                {
                    printf(
                        "Error: too many race values.\n");

                    return 0;
                }

                snprintf(
                    args->race[args->race_size],
                    MAX_COLOR_LEN,
                    "%s",
                    argv[i]);

                args->race_size++;

                i++;
            }

            continue;
        }

        /*------------------------------------------
            UNKNOWN
        ------------------------------------------*/

        printf(
            "Error: unknown argument \"%s\".\n",
            argv[i]);

        return 0;
    }

    /*
        image/palette only make sense together
        with --add_card
    */

    if (!args->add_card &&
        (args->image_given ||
         args->palette_given))
    {
        printf(
            "Error: image_id/palette require --add_card.\n");

        return 0;
    }

    return 1;
}

/*=========================================================
    Card insertion
=========================================================*/

static void append_card_to_database(
    cJSON *root,
    ParsedArguments *args)
{
    cJSON *cards =
        cJSON_GetObjectItem(root, "cards");

    if (!cJSON_IsArray(cards))
        return;

    cJSON *card =
        cJSON_CreateObject();

    /*-----------------------------------------------------
        Basic card information
    -----------------------------------------------------*/

    cJSON_AddStringToObject(
        card,
        "name",
        args->name);

    cJSON_AddNumberToObject(
        card,
        "image_id",
        args->image_id);

    /*-----------------------------------------------------
        Color palette
    -----------------------------------------------------*/

    cJSON *palette =
        cJSON_CreateArray();

    for (int i = 0;
         i < args->palette_size;
         i++)
    {
        cJSON_AddItemToArray(
            palette,
            cJSON_CreateString(
                args->palette[i]));
    }

    cJSON_AddItemToObject(
        card,
        "color_palette",
        palette);

    /*-----------------------------------------------------
        Card type
    -----------------------------------------------------*/

    cJSON_AddStringToObject(
        card,
        "card_type",
        args->card_type);

    /*-----------------------------------------------------
        Combat stats
    -----------------------------------------------------*/

    cJSON_AddNumberToObject(
        card,
        "atk",
        args->atk);

    cJSON_AddNumberToObject(
        card,
        "hp",
        args->hp);

    cJSON_AddNumberToObject(
        card,
        "cost",
        args->cost);

    cJSON_AddStringToObject(
        card,
        "atk_type",
        args->atk_type);

    /*-----------------------------------------------------
        Race
    -----------------------------------------------------*/

    cJSON *race =
        cJSON_CreateArray();

    for (int i = 0;
         i < args->race_size;
         i++)
    {
        cJSON_AddItemToArray(
            race,
            cJSON_CreateString(
                args->race[i]));
    }

    cJSON_AddItemToObject(
        card,
        "race",
        race);

    /*-----------------------------------------------------
        Abilities
    -----------------------------------------------------*/

    cJSON *abilities =
        cJSON_CreateArray();

    for (int i = 0;
         i < args->ability_count;
         i++)
    {
        Ability *ability =
            &args->abilities[i];

        cJSON *ability_json =
            cJSON_CreateObject();

        /*
            Every ability always has a type.
        */

        cJSON_AddStringToObject(
            ability_json,
            "type",
            ability->type);

        /*
            Abilities without parameters.

            Example:

                "First Strike"
        */

        if (ability->value_type ==
            ABILITY_VALUE_NONE)
        {
            /*
                Nothing else needs to be added.
            */
        }

        /*
            Abilities with one integer.

            Example:

                "Berserk 2"

            becomes:

                {
                    "type": "Berserk",
                    "value": 2
                }
        */

        else if (ability->value_type ==
                 ABILITY_VALUE_NUMBER)
        {
            cJSON_AddNumberToObject(
                ability_json,
                "value",
                ability->number);
        }

        /*
            Abilities with one string.

            Example:

                "Call Kin Green Dragon"

            becomes:

                {
                    "type": "Call Kin",
                    "value": "Green Dragon"
                }
        */

        else if (ability->value_type ==
                 ABILITY_VALUE_STRING)
        {
            cJSON_AddStringToObject(
                ability_json,
                "value",
                ability->string);
        }

        /*
            Abilities with multiple strings.

            Example:

                "Status Immunity Burn Poison"

            becomes:

                {
                    "type": "Status Immunity",
                    "value": [
                        "Burn",
                        "Poison"
                    ]
                }
        */

        else if (ability->value_type ==
                 ABILITY_VALUE_ARRAY)
        {
            cJSON *values =
                cJSON_CreateArray();

            for (int j = 0;
                 j < ability->value_count;
                 j++)
            {
                cJSON_AddItemToArray(
                    values,
                    cJSON_CreateString(
                        ability->values[j]));
            }

            cJSON_AddItemToObject(
                ability_json,
                "value",
                values);
        }

        cJSON_AddItemToArray(
            abilities,
            ability_json);
    }

    cJSON_AddItemToObject(
        card,
        "abilities",
        abilities);

    /*-----------------------------------------------------
        Optional effects
    -----------------------------------------------------*/

    if (args->on_cast_given)
    {
        cJSON_AddNumberToObject(
            card,
            "on_cast",
            args->on_cast);
    }

    if (args->on_death_given)
    {
        cJSON_AddNumberToObject(
            card,
            "on_death",
            args->on_death);
    }

    /*-----------------------------------------------------
        Add card to database
    -----------------------------------------------------*/

    cJSON_AddItemToArray(
        cards,
        card);
}

/*=========================================================
    Main
=========================================================*/

int main(int argc, char **argv)
{
    ParsedArguments args;

    /*
        No arguments?
        Nothing to do.
    */

    if (argc == 1)
        return 0;

    /*
        Parse command line.
    */

    if (!parse_arguments(argc, argv, &args))
        return 1;

    /*
        At the moment the only supported
        operation is adding a card.
    */

    if (!args.add_card)
        return 0;

    /*
        Load (or create) database.
    */

    cJSON *root = load_database();

    if (!root)
    {
        printf("Error: could not create database.\n");
        return 1;
    }

    /*
        Append the card.
    */

    append_card_to_database(
        root,
        &args);

    /*
        Save database.
    */

    if (!write_json_file(root))
    {
        printf("Error: could not write \"%s\".\n",
               CARD_INFO_FILE);

        cJSON_Delete(root);

        return 1;
    }

    printf("Added card \"%s\"\n", args.name);

    if (args.image_given)
        printf("  image_id = %d\n",
               args.image_id);

    if (args.palette_size > 0)
    {
        printf("  palette  = ");

        for (int i = 0; i < args.palette_size; i++)
        {
            if (i)
                printf(", ");

            printf("%s",
                   args.palette[i]);
        }

        printf("\n");
    }

    cJSON_Delete(root);

    return 0;
}