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

#include "cJSON.h"

#define CARD_INFO_FILE "src/card_info.json"

#define MAX_NAME_LEN 256
#define MAX_COLORS 32
#define MAX_COLOR_LEN 32

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
            "brown"};

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

} ParsedArguments;

/*-------------------------------------------------------*/

static void init_arguments(ParsedArguments *args)
{
    memset(args, 0, sizeof(*args));

    args->image_id = -1;
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
    const ParsedArguments *args)
{
    cJSON *cards = cJSON_GetObjectItem(root, "cards");

    if (!cJSON_IsArray(cards))
        return;

    cJSON *card = cJSON_CreateObject();

    cJSON_AddStringToObject(
        card,
        "name",
        args->name);

    cJSON_AddNumberToObject(
        card,
        "image_id",
        args->image_id);

    cJSON *palette =
        cJSON_CreateArray();

    for (int i = 0; i < args->palette_size; i++)
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