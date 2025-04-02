#include <iostream>
#include <iomanip>

#include "card.h"
#include "player.h"
#include "board.h"

using namespace std;

#define TL '.'
#define TR '.'
#define BL '.'
#define BR '.'
#define H '-'
#define V '|'
#define T 202
#define B 203
#define L 204
#define R 185

#define CARDWIDTH 16
#define CARDHEIGHT 8


Board::Board(){
    int i;
    // probably place an auxiliary array of bools to track which positions have a card placed in them or not.
    for (i = 0; i < 4; i++)
        PlayerCreatures[i] = Card("Empty", 0);
    for (i = 0; i < 4; i++)
        EnemyCreatures[i] = Card("Empty", 0);
    
}

void Board::PlacePlayerCreature(Player p, int card_idx){

    int i;
    for (i = 0; i < 4; i++)
    {
        if (this->PlayerCreatures[i].isValid() == false){
            this->PlayerCreatures[i] = p.getHandCard(card_idx);
            // must remove the card from the player's hand also
        }
    }
}

void moveTo(int x, int y)
{
    // ANSI escape code to move the cursor to (x, y)
    cout << "\033[" << y << ";" << x << "H";
}

void clearScreen()
{
    // ANSI escape code to clear the screen
    cout << "\033[2J\033[1;1H";
}

void drawBox(int x, int y, int width, int height)
{
    moveTo(x, y);
    wcout << static_cast<char>(TL);
    for (int i = 0; i < width; ++i)
        wcout << H;
    wcout << static_cast<char>(TR);

    for (int i = 1; i < height; ++i)
    {
        moveTo(x, y + i);
        cout << static_cast<char>(V);
        moveTo(x + width + 1, y + i);
        cout << static_cast<char>(V);
    }

    moveTo(x, y + height);
    cout << static_cast<char>(BL);
    for (int i = 0; i < width; ++i)
        cout << static_cast<char>(H);
    cout << static_cast<char>(BR);
}

void drawCard(int x, int y, Card c)
{
    drawBox(x, y, CARDWIDTH, CARDHEIGHT);
    drawBox(x, y + 3, CARDWIDTH, CARDHEIGHT - 3);
    moveTo(x + CARDWIDTH / 2 - c.getName().length() / 2 + (c.getName().length() % 2 == 0 ? 1 : 0), y + 1);
    cout << c.getName() << endl;
    moveTo(x + CARDWIDTH / 2 - c.getTitle().length() / 2, y + 2);
    cout << (c.getTitle() == "None" ? "" : c.getTitle()) << endl;
    moveTo(x + 1, y + 4);
    cout << "\033[1;31mAtk: " << c.getAttack() << endl;
    moveTo(x + 1, y + 5);
    cout << "\033[1;32mHP: " << c.getHP() << endl;
    moveTo(x + 1, y + 6);
    cout << "\033[1;33mCost: " << c.getCost() << "\033[0m" << endl;
}

void drawLastPlayedCardZones(int x, int y)
{
    // Last played card
    drawBox(x, y, CARDWIDTH + 2, CARDHEIGHT + 2);
    drawBox(x, y + CARDHEIGHT + 2, CARDWIDTH + 2, CARDHEIGHT + 2);
}

void drawHandZone(int x, int y, Player p)
{

    int s = CARDWIDTH + 2;
    cout << "\033[1;90m";
    drawBox(x + 2 * s, y, CARDWIDTH + 2, CARDHEIGHT + 2);
    drawBox(x + 3 * s + 1, y, CARDWIDTH + 2, CARDHEIGHT + 2);
    drawBox(x + 4 * s + 2, y, CARDWIDTH + 2, CARDHEIGHT + 2);
    drawBox(x + 5 * s + 3, y, CARDWIDTH + 2, CARDHEIGHT + 2);
    cout << "\033[1;0m";
    drawCard(x + 2 * s + 1, y + 1, p.getHandCard(0));
    drawCard(x + 3 * s + 2, y + 1, p.getHandCard(1));
    drawCard(x + 4 * s + 3, y + 1, p.getHandCard(2));
    drawCard(x + 5 * s + 4, y + 1, p.getHandCard(3));
}

void drawCenterBoard(int x, int y)
{

    int s = CARDWIDTH + 2, h = CARDHEIGHT + 2;
    drawBox(x, y, s, h);
    drawBox(x + 1 * s + 1, y, s, h);
    drawBox(x + 2 * s + 2, y, s, h);
    drawBox(x + 3 * s + 3, y, s, h);
    drawBox(x, y + h, s, h);
    drawBox(x + 1 * s + 1, y + h, s, h);
    drawBox(x + 2 * s + 2, y + h, s, h);
    drawBox(x + 3 * s + 3, y + h, s, h);

    // Must draw all cards that were placed
}

void drawBoardTop(int x, int y)
{
    int s = CARDWIDTH + 2;
    cout << "\033[1;33m";
    drawBox(x, y, s, 2);
    drawBox(x + s + 1, y, s, 2);
    drawBox(x + 2 * s + 2, y, s, 2);
    drawBox(x + 3 * s + 3, y, s, 2);
    moveTo(x + s / 2, y + 1);
    cout << "1";
    moveTo(x + 3 * s / 2 + 1, y + 1);
    cout << "2";
    moveTo(x + 5 * s / 2 + 2, y + 1);
    cout << "3";
    moveTo(x + 7 * s / 2 + 3, y + 1);
    cout << "4";
    cout << "\033[1;0m";
}

void drawTurnCounterBoard(int x, int y) {

    int i;
    cout << "\033[1;90m";
    for (i = 0; i < 3; i++){
        moveTo(x + 1, y + 2*i);
        cout << "|               |";
        moveTo(x, y + 2*i + 1);
        cout << "[ ]             [ ]";
    }
    moveTo(x + 1, y + 6);
    cout << "|               |";
    cout << "\033[1;0m";
    drawBox(x, y + 7, 17, 4);
}

void drawBoard(int x, int y, Player p)
{

    int s = CARDWIDTH + 2, h = CARDHEIGHT + 2;
    drawLastPlayedCardZones(x + 3, y + 3);
    drawBoardTop(x + 2 * s, y);
    drawCenterBoard(x + 2 * s, y + 3);
    drawTurnCounterBoard(x + 6 * s + 8, y);
    drawHandZone(x, y + 2 * h + 4, p);
}

void updateTurnCounter(int x, int y, int *ctr){

    (*ctr)++;
    int s = CARDWIDTH + 2;
    moveTo(x + 6 * s + 16, y + 9);
    /* cout << *ctr; */
    cout << "\033[1;33m";
    std::cout << setw(2) << setfill(' ') << *ctr << endl;
    cout << "\033[1;0m";
}