#include <iostream>
#include <string>
#include "card.h"
#include "player.h"

using namespace std;

Player::Player(string name)
: name(name), mana(5), maxMana(50), health(50), maxHealth(50) {

}

void Player::setHPMana(int hp, int mana) {
    this->health = hp;
    this->maxHealth = hp;
    this->mana = mana;
    this->maxMana = mana;
}

void Player::setDeck(Card deck[]) {
    for (int i = 0; i < DECKSIZE; i++) {
        this->deck[i] = deck[i];
    }
}

void Player::drawCards(int numCards) {
    int j = 0;
    for (int i = 0; i < numCards; i++) {
        j++;
        if (j > HANDSIZE) {
            cout << "Hand is full!" << endl;
            break;
        }
        if (this->hand[j-1].getName() != "NO NAME"){
            i--;
            continue;
        }
        this->hand[j-1] = this->deck[i];
    }
}

Card Player::getHandCard(int index) {
    if (index < 0 || index >= HANDSIZE) {
        cout << "Invalid index!" << endl;
        return Card(); // Return an empty card
    }
    return this->hand[index];
}