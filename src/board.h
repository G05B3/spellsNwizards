#include "card.h"
#include "player.h"

#ifndef BOARD_H
#define BOARD_H


class Board{
    public:
        Board();
        void PlacePlayerCreature(Player p, int card_idx);
    private:
        Card PlayerCreatures[4];
        Card EnemyCreatures[4];
};

void clearScreen();
void drawCard(int x, int y, Card c);
void drawHandZone(int x, int y, Player p);
void drawCenterBoard(int x, int y);
void drawBoard(int x, int y, Player p);
void updateTurnCounter(int x, int y, int *ctr);

#endif