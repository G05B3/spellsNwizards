#include "cards.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

/*=========================================================
    Helpers
=========================================================*/

static void init_card(Card *card)
{
    int i;

    card->id = -1;
    card->image_id = -1;

    card->name[0] = '\0';

    for (i = 0; i < PALETTE_SIZE; i++)
        card->palette[i] = WHITE;
}


/*-------------------------------------------------------*/

static int parse_card(cJSON *json, Card *card)
{
    int i;

    cJSON *name;
    cJSON *image;
    cJSON *palette;

    init_card(card);

    name = cJSON_GetObjectItem(json, "name");
    image = cJSON_GetObjectItem(json, "image_id");
    palette = cJSON_GetObjectItem(json, "color_palette");

    if (!cJSON_IsString(name))
        return 0;

    snprintf(card->name,
             sizeof(card->name),
             "%s",
             name->valuestring);

    if (cJSON_IsNumber(image))
        card->image_id = image->valueint;

    if (cJSON_IsArray(palette))
    {
        int count = cJSON_GetArraySize(palette);

        if (count > PALETTE_SIZE)
            count = PALETTE_SIZE;

        for (i = 0; i < count; i++)
        {
            cJSON *color =
                cJSON_GetArrayItem(palette, i);

            if (!cJSON_IsString(color))
                continue;

            int c = color_from_name(color->valuestring);

            if (c >= 0)
                card->palette[i] = (Color)c;
        }
    }

    return 1;
}


/*=========================================================
    Public Functions
=========================================================*/

int load_cards(CardDatabase *db, const char *filename)
{
    FILE *f;

    long size;

    char *buffer;

    cJSON *root;
    cJSON *array;

    int i;

    memset(db, 0, sizeof(*db));
    db->card_count = 0;

    f = fopen(filename, "rb");

    if (!f)
        return 0;

    fseek(f, 0, SEEK_END);

    size = ftell(f);

    rewind(f);

    buffer = malloc(size + 1);

    if (!buffer)
    {
        fclose(f);
        return 0;
    }

    fread(buffer, 1, size, f);

    buffer[size] = '\0';

    fclose(f);

    root = cJSON_Parse(buffer);

    free(buffer);

    if (!root)
        return 0;

    array = cJSON_GetObjectItem(root, "cards");

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
            cJSON_GetArrayItem(array, i);

        if (!cJSON_IsObject(entry))
            continue;

        if (!parse_card(entry,
                        &db->cards[db->card_count]))
            continue;

        db->cards[db->card_count].id =
            db->card_count;

        db->card_count++;
    }

    cJSON_Delete(root);

    return 1;
}


/*-------------------------------------------------------*/

Card *get_card(CardDatabase *db, int id)
{
    if (id < 0 || id >= db->card_count)
        return NULL;

    return &db->cards[id];
}