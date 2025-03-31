#include <iostream>
#include <string>
#include "card.h"

using namespace std;

#ifndef PLAYER_H
#define PLAYER_H

#define DECKSIZE 40
#define HANDSIZE 4

class Player {
    public:
        Player(string name);
        void setHPMana(int hp, int mana);
        void drawCards(int numCards);
        void setDeck(Card deck[]);
        Card getHandCard(int index);
    private:
        string name;
        int mana;
        int maxMana;
        int health;
        int maxHealth;
        Card deck[DECKSIZE];
        Card hand[HANDSIZE];
};


#endif