#include "deck.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

#include "access.h"
#include "card_artwork.h"
#include "cJSON.h"


void shuffle_deck(Deck *deck)
{
    int i;

    for (i = DECK_SIZE - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        Card *temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
}

/*=========================================================
    Internal Helpers
=========================================================*/

static Card *find_card(
    CardDatabase *db,
    const char *name)
{
    int i;

    for (i = 0; i < db->card_count; i++)
    {
        if (strcmp(db->cards[i].name, name) == 0)
            return &db->cards[i];
    }

    return NULL;
}


static int get_copy_limit(
    const Card *card)
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


static int has_json_extension(
    const char *name)
{
    const char *dot;

    dot = strrchr(name, '.');

    if (!dot)
        return 0;

    return strcmp(dot, ".json") == 0;
}


static char *read_file(
    const char *filename)
{
    FILE *f;
    long size;
    char *buffer;

    f = fopen(filename, "rb");

    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    if (size < 0)
    {
        fclose(f);
        return NULL;
    }

    buffer = malloc((size_t)size + 1);

    if (!buffer)
    {
        fclose(f);
        return NULL;
    }

    if (fread(buffer, 1, (size_t)size, f) != (size_t)size)
    {
        free(buffer);
        fclose(f);
        return NULL;
    }

    buffer[size] = '\0';

    fclose(f);

    return buffer;
}


/*=========================================================
    Deck Validation
=========================================================*/

int validate_deck(
    const Deck *deck)
{
    int counts[MAX_CARDS] = {0};
    int i;
    int total = 0;

    if (!deck)
        return 0;

    /*
        A deck must have exactly DECK_SIZE cards.
    */
    for (i = 0; i < DECK_SIZE; i++)
    {
        if (!deck->cards[i])
            return 0;

        total++;
    }

    if (total != DECK_SIZE)
        return 0;

    /*
        Front card must exist.
    */
    if (!deck->front_card)
        return 0;

    /*
        Count copies by card ID.
    */
    for (i = 0; i < DECK_SIZE; i++)
    {
        Card *card = deck->cards[i];

        if (card->id < 0 || card->id >= MAX_CARDS)
            return 0;

        counts[card->id]++;
    }

    /*
        Check per-card copy limits.
    */
    for (i = 0; i < MAX_CARDS; i++)
    {
        if (counts[i] == 0)
            continue;

        /*
            We need the actual card to determine its type.
            Since card IDs index the database in the current
            implementation, the card pointer from the deck
            is enough to check the limit below.
        */
        {
            int j;

            for (j = 0; j < DECK_SIZE; j++)
            {
                Card *card = deck->cards[j];

                if (card->id == i)
                {
                    int limit = get_copy_limit(card);

                    if (counts[i] > limit)
                        return 0;

                    break;
                }
            }
        }
    }

    return 1;
}


/*=========================================================
    Load One Deck
=========================================================*/

int load_deck(
    Deck *deck,
    CardDatabase *cards,
    const char *filename)
{
    char *text;
    cJSON *root;
    cJSON *name;
    cJSON *front;
    cJSON *card_array;

    int i;
    int card_count;

    int copy_counts[MAX_CARDS] = {0};

    if (!deck || !cards || !filename)
        return 0;

    memset(deck, 0, sizeof(Deck));

    text = read_file(filename);

    if (!text)
    {
        printf(
            "Warning: could not read deck '%s'.\n",
            filename);

        return 0;
    }

    root = cJSON_Parse(text);

    free(text);

    if (!root)
    {
        printf(
            "Warning: invalid JSON in '%s'.\n",
            filename);

        return 0;
    }


    /*-----------------------------------------------------
        Name
    -----------------------------------------------------*/

    name =
        cJSON_GetObjectItem(
            root,
            "name");

    if (!cJSON_IsString(name) ||
        name->valuestring[0] == '\0')
    {
        printf(
            "Warning: deck '%s' has no valid name.\n",
            filename);

        cJSON_Delete(root);
        return 0;
    }

    strncpy(
        deck->name,
        name->valuestring,
        MAX_DECK_NAME - 1);

    deck->name[MAX_DECK_NAME - 1] = '\0';


    /*-----------------------------------------------------
        Front card
    -----------------------------------------------------*/

    front =
        cJSON_GetObjectItem(
            root,
            "front_card");

    if (!cJSON_IsString(front) ||
        front->valuestring[0] == '\0')
    {
        printf(
            "Warning: deck '%s' has no valid front_card.\n",
            filename);

        cJSON_Delete(root);
        return 0;
    }

    deck->front_card =
        find_card(
            cards,
            front->valuestring);

    if (!deck->front_card)
    {
        printf(
            "Warning: deck '%s' references unknown "
            "front card '%s'.\n",
            filename,
            front->valuestring);

        cJSON_Delete(root);
        return 0;
    }


    /*-----------------------------------------------------
        Card array
    -----------------------------------------------------*/

    card_array =
        cJSON_GetObjectItem(
            root,
            "cards");

    if (!cJSON_IsArray(card_array))
    {
        printf(
            "Warning: deck '%s' has no valid cards array.\n",
            filename);

        cJSON_Delete(root);
        return 0;
    }

    card_count =
        cJSON_GetArraySize(card_array);

    if (card_count != DECK_SIZE)
    {
        printf(
            "Warning: deck '%s' contains %d cards; "
            "expected exactly %d.\n",
            filename,
            card_count,
            DECK_SIZE);

        cJSON_Delete(root);
        return 0;
    }


    /*-----------------------------------------------------
        Parse cards
    -----------------------------------------------------*/

    for (i = 0; i < DECK_SIZE; i++)
    {
        cJSON *entry;
        Card *card;
        int limit;

        entry =
            cJSON_GetArrayItem(
                card_array,
                i);

        if (!cJSON_IsString(entry))
        {
            printf(
                "Warning: deck '%s' contains a "
                "non-string card entry.\n",
                filename);

            cJSON_Delete(root);
            return 0;
        }

        card =
            find_card(
                cards,
                entry->valuestring);

        if (!card)
        {
            printf(
                "Warning: deck '%s' references unknown "
                "card '%s'.\n",
                filename,
                entry->valuestring);

            cJSON_Delete(root);
            return 0;
        }

        limit =
            get_copy_limit(card);

        if (limit <= 0)
        {
            printf(
                "Warning: card '%s' has no valid "
                "deck copy limit.\n",
                card->name);

            cJSON_Delete(root);
            return 0;
        }

        copy_counts[card->id]++;

        if (copy_counts[card->id] > limit)
        {
            printf(
                "Warning: deck '%s' contains too many "
                "copies of '%s' (maximum %d).\n",
                filename,
                card->name,
                limit);

            cJSON_Delete(root);
            return 0;
        }

        deck->cards[i] = card;
    }


    cJSON_Delete(root);

    return validate_deck(deck);
}


/*=========================================================
    Load All Decks
=========================================================*/

int load_decks(
    DeckDatabase *db,
    CardDatabase *cards,
    const char *path)
{
    DIR *dir;
    struct dirent *entry;

    if (!db || !cards || !path)
        return 0;

    db->deck_count = 0;

    dir = opendir(path);

    if (!dir)
    {
        printf(
            "ERROR: could not open deck directory '%s'.\n",
            path);

        return 0;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        char filename[512];
        Deck deck;

        if (!has_json_extension(entry->d_name))
            continue;

        if (db->deck_count >= MAX_DECKS)
        {
            printf(
                "Warning: maximum number of decks "
                "reached (%d).\n",
                MAX_DECKS);

            break;
        }

        snprintf(
            filename,
            sizeof(filename),
            "%s/%s",
            path,
            entry->d_name);

        if (!load_deck(
                &deck,
                cards,
                filename))
        {
            /*
                Invalid decks are simply ignored.
            */
            continue;
        }

        db->decks[db->deck_count++] = deck;
    }

    closedir(dir);

    return 1;
}

/*=========================================================
    Draw Deck
=========================================================*/

void draw_deck(
    int x,
    int y,
    const Deck *deck,
    int selected)
{
    Card front;
    Card dummy;

    if (!deck || !deck->front_card)
        return;

    /*
        ---------------------------------------------------
        Prepare front card
        ---------------------------------------------------
    */

    front = *deck->front_card;

    /*
        Unselected decks are completely grayscale.
    */
    if (!selected)
    {
        int i;

        for (i = 0; i < PALETTE_SIZE; i++)
            front.palette[i] = GRAY;
    }


    /*
        ---------------------------------------------------
        Dummy card
        ---------------------------------------------------

        Copy the card so that its normal outline/artwork
        machinery is still used, but make the image blank.

        We will draw it twice behind the real card.
    */

    dummy = front;
    dummy.image_id = -1;
    dummy.card_type = CARD_NONE;

    /*
        Remove the information that we don't want displayed.
    */

    dummy.cost = 0;
    dummy.atk  = 0;
    dummy.hp   = 0;


    /*
        ---------------------------------------------------
        Back card #2
        ---------------------------------------------------
    */

    draw_card(
        x + 2,
        y + 2,
        &dummy);


    /*
        ---------------------------------------------------
        Back card #1
        ---------------------------------------------------
    */

    draw_card(
        x + 1,
        y + 1,
        &dummy);


    /*
        ---------------------------------------------------
        Actual front card
        ---------------------------------------------------
    */

    front.cost = 0;
    front.atk  = 0;
    front.hp   = 0;
    front.card_type = CARD_NONE;

    draw_card(
        x,
        y,
        &front);


    /*
        ---------------------------------------------------
        Replace the card's name with the deck name
        ---------------------------------------------------

        draw_card() has already drawn the card name, so
        overwrite that region with the deck name.

        We clear the complete name line first so that a
        shorter deck name cannot leave remnants behind.
    */

    set_color(
        selected ? WHITE : GRAY);

    gotoxy(
        x + 1,
        y + 1);

    printf(
        "%-*s",
        CLENGTH,
        "");

    gotoxy(
        x + CLENGTH / 2 -
        (int)strlen(deck->name) / 2,
        y + 1);

    printf(
        "%s",
        deck->name);

    reset_color();
}


/*=========================================================
    Keyboard
=========================================================*/

static int read_key(void)
{
    struct termios old_term;
    struct termios new_term;

    int c;

    tcgetattr(
        STDIN_FILENO,
        &old_term);

    new_term = old_term;

    new_term.c_lflag &=
        ~(ICANON | ECHO);

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &new_term);

    c = getchar();

    /*
        Arrow keys arrive as:

            ESC [ D
            ESC [ C
    */

    if (c == 27)
    {
        int c2 = getchar();

        if (c2 == '[')
        {
            int c3 = getchar();

            if (c3 == 'D')
                c = 'L';

            else if (c3 == 'C')
                c = 'R';

            else
                c = 27;
        }
        else
        {
            c = 27;
        }
    }

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &old_term);

    return c;
}


/*=========================================================
    Deck Selector
=========================================================*/

int deck_selector(
    const DeckDatabase *db)
{
    int selected;
    int running;

    int card_width;
    int spacing;

    int center_x;
    int left_x;
    int right_x;

    int y;

    int key;

    if (!db || db->deck_count <= 0)
        return -1;

    selected = 0;
    running = 1;

    card_width =
        CLENGTH + 2;

    spacing = 6;

    /*
        The center deck is centered in the terminal.
    */
    center_x =
        terminal_width / 2 -
        card_width / 2;

    y = 4;

    hide_cursor();

    while (running)
    {
        clear_screen();
        get_terminal_size();

        center_x =
            terminal_width / 2 -
            card_width / 2;

        /*
            Previous deck.
        */
        if (selected > 0)
        {
            left_x =
                center_x -
                card_width -
                spacing;

            draw_deck(
                left_x,
                y,
                &db->decks[selected - 1],
                0);
        }

        /*
            Selected deck.
        */
        draw_deck(
            center_x,
            y,
            &db->decks[selected],
            1);

        /*
            Next deck.
        */
        if (selected < db->deck_count - 1)
        {
            right_x =
                center_x +
                card_width +
                spacing;

            draw_deck(
                right_x,
                y,
                &db->decks[selected + 1],
                0);
        }

        /*
            Controls.
        */
        set_color(WHITE);

        gotoxy(
            terminal_width / 2 - 18,
            y + CHEIGHT + 7);

        printf(
            "A / D or arrows: select");

        gotoxy(
            terminal_width / 2 - 9,
            y + CHEIGHT + 8);

        printf(
            "ENTER: choose");

        gotoxy(
            terminal_width / 2 - 8,
            y + CHEIGHT + 9);

        printf(
            "ESC: cancel");

        reset_color();

        key = read_key();

        switch (key)
        {
            case 'a':
            case 'A':
            case 'L':
                if (selected > 0)
                    selected--;

                break;

            case 'd':
            case 'D':
            case 'R':
                if (selected < db->deck_count - 1)
                    selected++;

                break;

            case '\n':
            case '\r':
                running = 0;
                break;

            case 27:
                show_cursor();
                clear_screen();
                return -1;
        }
    }

    show_cursor();
    clear_screen();

    return selected;
}


/*=========================================================
    Free
=========================================================*/

void free_decks(
    DeckDatabase *db)
{
    if (!db)
        return;

    /*
        Cards themselves belong to CardDatabase, so there is
        nothing to free inside each Deck.
    */

    db->deck_count = 0;
}