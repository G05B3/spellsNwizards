#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>

#include "cards.h"
#include "cJSON.h"

/*=========================================================*
* Constants
*=========================================================*/

#define CARD_DATABASE_FILE "src/card_info.json"
#define DECK_OUTPUT_DIR    "decks"

#define DECK_SIZE          40

/*
    Maximum number of copies of a card depending
    on its card type.

    Change these for testing if necessary.
*/

#define MAX_CREATURE_COPIES     3
#define MAX_SPELL_COPIES        3
#define MAX_ARTIFACT_COPIES     1
#define MAX_ENVIRONMENT_COPIES  1

#define MAX_DECK_NAME           128
#define MAX_INPUT_LINE          512
#define MAX_FILENAME            256


/*=========================================================*
* Helpers
*=========================================================*/

/*
    Removes leading and trailing whitespace.
*/
static char *trim(char *str)
{
    char *end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == '\0')
        return str;

    end = str + strlen(str) - 1;

    while (end > str &&
           isspace((unsigned char)*end))
    {
        end--;
    }

    end[1] = '\0';

    return str;
}


/*---------------------------------------------------------*/

static int get_copy_limit(const Card *card)
{
    switch (card->card_type)
    {
        case CARD_CREATURE:
            return MAX_CREATURE_COPIES;

        case CARD_SPELL:
            return MAX_SPELL_COPIES;

        case CARD_ARTIFACT:
            return MAX_ARTIFACT_COPIES;

        case CARD_ENVIRONMENT:
            return MAX_ENVIRONMENT_COPIES;

        default:
            return 0;
    }
}


/*---------------------------------------------------------*/

/*
    Finds a card by name.

    Returns NULL if the card does not exist.
*/
static Card *find_card(
    CardDatabase *db,
    const char *name)
{
    int i;

    for (i = 0; i < db->card_count; i++)
    {
        if (!strcmp(db->cards[i].name, name))
            return &db->cards[i];
    }

    return NULL;
}


/*---------------------------------------------------------*/

/*
    Converts a deck name into a reasonably safe filename.

    For example:

        "My First Deck"
            ->
        "my_first_deck"
*/
static void make_filename(
    const char *name,
    char *filename,
    size_t size)
{
    size_t i;
    size_t j = 0;

    for (i = 0;
         name[i] != '\0' &&
         j + 1 < size;
         i++)
    {
        unsigned char c =
            (unsigned char)name[i];

        if (isalnum(c))
        {
            filename[j++] =
                (char)tolower(c);
        }
        else if (c == ' ' ||
                 c == '_' ||
                 c == '-')
        {
            /*
                Avoid repeated underscores.
            */

            if (j > 0 &&
                filename[j - 1] != '_')
            {
                filename[j++] = '_';
            }
        }
    }

    /*
        Remove trailing underscore.
    */

    if (j > 0 &&
        filename[j - 1] == '_')
    {
        j--;
    }

    filename[j] = '\0';

    /*
        Empty names are unlikely, but avoid
        generating an empty filename.
    */

    if (j == 0)
    {
        snprintf(
            filename,
            size,
            "deck");
    }
}


/*=========================================================*
* Input Parsing
*=========================================================*/

static int parse_deck_input(
    const char *filename,
    CardDatabase *db,
    char *deck_name,
    size_t deck_name_size,
    int *front_card_image_id,
    Card **deck_cards)
{
    FILE *f;

    char line[MAX_INPUT_LINE];

    int deck_count = 0;

    char front_card_name[256];

    int front_card_found = 0;

    int copy_counts[MAX_CARDS];

    int i;

    memset(
        copy_counts,
        0,
        sizeof(copy_counts));

    front_card_name[0] = '\0';

    deck_name[0] = '\0';

    *front_card_image_id = -1;

    f = fopen(filename, "r");

    if (!f)
    {
        printf(
            "ERROR: could not open input file '%s'.\n",
            filename);

        return 0;
    }

    while (fgets(line, sizeof(line), f))
    {
        char *text;

        text = trim(line);

        /*
            Ignore blank lines.
        */

        if (*text == '\0')
            continue;

        /*
            Ignore comments.
        */

        if (*text == '#')
            continue;


        /*-------------------------------------------------
            Deck name
        -------------------------------------------------*/

        if (!strncmp(
                text,
                "name:",
                5))
        {
            char *value =
                trim(text + 5);

            snprintf(
                deck_name,
                deck_name_size,
                "%s",
                value);

            continue;
        }


        /*-------------------------------------------------
            Front card
        -------------------------------------------------*/

        if (!strncmp(
                text,
                "front_card:",
                11))
        {
            char *value =
                trim(text + 11);

            snprintf(
                front_card_name,
                sizeof(front_card_name),
                "%s",
                value);

            continue;
        }


        /*-------------------------------------------------
            Card entry

            Expected:

                3x Warrior
                2x Blue Mage
        -------------------------------------------------*/

        {
            char *p = text;

            char *end;
            char *card_name;

            long amount;

            /*
                Parse the number.
            */

            if (!isdigit((unsigned char)*p))
            {
                printf(
                    "Warning: unrecognized line: %s\n",
                    text);

                continue;
            }

            amount = strtol(p, &end, 10);

            if (end == p)
            {
                printf(
                    "Warning: invalid card entry: %s\n",
                    text);

                continue;
            }

            /*
                Skip whitespace.
            */

            p = end;

            while (isspace((unsigned char)*p))
                p++;

            /*
                Require 'x' or 'X'.
            */

            if (*p != 'x' &&
                *p != 'X')
            {
                printf(
                    "Warning: expected 'Nx Card Name': %s\n",
                    text);

                continue;
            }

            p++;

            /*
                Skip whitespace after x.
            */

            while (isspace((unsigned char)*p))
                p++;

            card_name = trim(p);

            if (*card_name == '\0')
            {
                printf(
                    "Warning: missing card name: %s\n",
                    text);

                continue;
            }

            if (amount <= 0)
            {
                printf(
                    "Warning: invalid copy count for '%s'.\n",
                    card_name);

                continue;
            }

            /*
                Find card in database.
            */

            Card *card =
                find_card(db, card_name);

            if (!card)
            {
                printf(
                    "Warning: card '%s' not found "
                    "in card database. Ignoring.\n",
                    card_name);

                continue;
            }

            /*
                Determine maximum allowed copies.
            */

            int limit =
                get_copy_limit(card);

            int available =
                limit - copy_counts[card->id];

            if (available <= 0)
            {
                printf(
                    "Warning: '%s' already has "
                    "the maximum number of copies (%d). "
                    "Ignoring extra copies.\n",
                    card->name,
                    limit);

                continue;
            }

            /*
                Do not allow a card entry to exceed
                its copy limit.
            */

            if (amount > available)
            {
                printf(
                    "Warning: '%s' requested %ld copies, "
                    "but only %d are allowed. "
                    "Adding %d copies.\n",
                    card->name,
                    amount,
                    limit,
                    available);

                amount = available;
            }

            /*
                Do not overflow the deck.
            */

            if (deck_count + amount > DECK_SIZE)
            {
                int remaining =
                    DECK_SIZE - deck_count;

                printf(
                    "Warning: deck can only contain "
                    "%d more card(s). Adding %d copies "
                    "of '%s'.\n",
                    remaining,
                    remaining,
                    card->name);

                amount = remaining;
            }

            /*
                Add copies.
            */

            for (i = 0; i < amount; i++)
            {
                deck_cards[deck_count++] =
                    card;
            }

            copy_counts[card->id] += amount;

            /*
                Deck is already full.
            */

            if (deck_count >= DECK_SIZE)
                break;
        }
    }

    fclose(f);


    /*-----------------------------------------------------
        Validate deck name
    -----------------------------------------------------*/

    if (deck_name[0] == '\0')
    {
        printf(
            "ERROR: deck has no name.\n");

        return 0;
    }


    /*-----------------------------------------------------
        Validate front card
    -----------------------------------------------------*/

    if (front_card_name[0] == '\0')
    {
        printf(
            "ERROR: deck has no front_card.\n");

        return 0;
    }

    Card *front_card =
        find_card(
            db,
            front_card_name);

    if (!front_card)
    {
        printf(
            "ERROR: front card '%s' "
            "was not found in the card database.\n",
            front_card_name);

        return 0;
    }

    *front_card_image_id =
        front_card->image_id;

    front_card_found = 1;


    /*-----------------------------------------------------
        Validate deck size
    -----------------------------------------------------*/

    printf(
        "\nDeck '%s'\n",
        deck_name);

    printf(
        "Cards: %d / %d\n",
        deck_count,
        DECK_SIZE);

    printf(
        "Front card: %s (image_id %d)\n",
        front_card->name,
        front_card->image_id);

    if (deck_count != DECK_SIZE)
    {
        printf(
            "ERROR: deck must contain exactly "
            "%d cards.\n",
            DECK_SIZE);

        return 0;
    }

    if (!front_card_found)
        return 0;

    return deck_count;
}


/*=========================================================*
* JSON Output
*=========================================================*/

static int write_deck_json(
    const char *deck_name,
    int front_card_image_id,
    Card **deck_cards)
{
    cJSON *root;
    cJSON *cards;

    char filename[MAX_FILENAME];

    char path[MAX_FILENAME + 32];

    char *json_text;

    FILE *f;

    int i;


    /*
        Create output directory if necessary.
    */

    if (mkdir(DECK_OUTPUT_DIR, 0755) != 0 &&
        errno != EEXIST)
    {
        printf(
            "ERROR: could not create '%s'.\n",
            DECK_OUTPUT_DIR);

        return 0;
    }


    /*
        Convert deck name into a filename.
    */

    make_filename(
        deck_name,
        filename,
        sizeof(filename));

    snprintf(
        path,
        sizeof(path),
        "%s/%s.json",
        DECK_OUTPUT_DIR,
        filename);


    /*
        Create JSON root.
    */

    root =
        cJSON_CreateObject();

    if (!root)
        return 0;


    /*
        Deck name.
    */

    cJSON_AddStringToObject(
        root,
        "name",
        deck_name);


    /*
        Front card artwork.
    */

    cJSON_AddNumberToObject(
        root,
        "front_card",
        front_card_image_id);


    /*
        Card array.
    */

    cards =
        cJSON_CreateArray();

    if (!cards)
    {
        cJSON_Delete(root);
        return 0;
    }

    cJSON_AddItemToObject(
        root,
        "cards",
        cards);


    /*
        Add card names.

        Duplicates are intentionally retained.
    */

    for (i = 0; i < DECK_SIZE; i++)
    {
        cJSON_AddItemToArray(
            cards,
            cJSON_CreateString(
                deck_cards[i]->name));
    }


    /*
        Convert JSON to text.
    */

    json_text =
        cJSON_Print(root);

    if (!json_text)
    {
        cJSON_Delete(root);
        return 0;
    }


    /*
        Write file.
    */

    f = fopen(path, "w");

    if (!f)
    {
        printf(
            "ERROR: could not create '%s'.\n",
            path);

        free(json_text);
        cJSON_Delete(root);

        return 0;
    }

    fprintf(
        f,
        "%s\n",
        json_text);

    fclose(f);

    free(json_text);
    cJSON_Delete(root);


    printf(
        "Deck successfully written to:\n"
        "    %s\n",
        path);

    return 1;
}


/*=========================================================*
* Main
*=========================================================*/

int main(
    int argc,
    char **argv)
{
    CardDatabase db;

    Card *deck_cards[DECK_SIZE];

    char deck_name[MAX_DECK_NAME];

    int front_card_image_id;

    int result;


    /*
        Check arguments.
    */

    if (argc != 2)
    {
        printf(
            "Usage:\n"
            "    %s <deck_input.txt>\n\n"
            "Example:\n"
            "    %s deck_inputs/my_deck.txt\n",
            argv[0],
            argv[0]);

        return 1;
    }


    /*-----------------------------------------------------
        Load card database
    -----------------------------------------------------*/

    printf(
        "Loading card database...\n");

    if (!load_cards(
            &db,
            CARD_DATABASE_FILE))
    {
        printf(
            "ERROR: failed to load '%s'.\n",
            CARD_DATABASE_FILE);

        return 1;
    }

    printf(
        "Loaded %d cards.\n",
        db.card_count);


    /*-----------------------------------------------------
        Parse deck
    -----------------------------------------------------*/

    result =
        parse_deck_input(
            argv[1],
            &db,
            deck_name,
            sizeof(deck_name),
            &front_card_image_id,
            deck_cards);

    if (!result)
        return 1;


    /*-----------------------------------------------------
        Write JSON
    -----------------------------------------------------*/

    if (!write_deck_json(
            deck_name,
            front_card_image_id,
            deck_cards))
    {
        return 1;
    }


    return 0;
}