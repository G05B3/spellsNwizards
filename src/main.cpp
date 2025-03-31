#include <iostream>
#include <string>
#include <map>

#include "card.h"
#include "board.h"
#include "player.h"

using namespace std;

int main(){
    
    std::cout << "Hello, World!" << std::endl;
    std::map<string, Card> cardDict = createDictionary();
    /* for (const auto& pair : cardDict) {
        pair.second.print();
    } */

    Player p = Player("Reinhart");
    int turn = 0;
    p.setHPMana(50, 50);

    Card deck[DECKSIZE];
    for (int i = 0; i < DECKSIZE; i+=2) {
        deck[i] = cardDict["Warrior"];
        deck[i + 1] = cardDict["Knight"];
    }
    p.setDeck(deck);

    p.drawCards(4);

    clearScreen();
    drawBoard(1, 1, p);
    
    updateTurnCounter(1, 1, &turn);

    return 0;
}