#ifndef ARTWORK_H
#define ARTWORK_H

#include "cards.h"

/*=========================================================
    Primitive Drawing
=========================================================*/

void draw_outline(
    int x,
    int y,
    Color color);

void draw_card(
    int x,
    int y,
    const Card *card);

void delete_card(
    int x,
    int y
);


/*=========================================================
    Artwork
=========================================================*/

void draw_image(
    int x,
    int y,
    const Card *card);


/*=========================================================
    Layout
=========================================================*/

void draw_all_cards(
    const CardDatabase *db);

#endif