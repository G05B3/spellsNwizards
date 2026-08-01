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


/*=========================================================
    Artwork
=========================================================*/

void draw_unknown(
    int x,
    int y,
    const Card *card);

void draw_green_dragon(
    int x,
    int y,
    const Card *card);

void draw_warrior(
    int x,
    int y,
    const Card *card);

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