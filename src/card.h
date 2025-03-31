#include <iostream>
#include <string>
#include <map>

using namespace std;

#ifndef CARD_H
#define CARD_H

#define MAX_ATTACK_TYPES 3
#define MAX_ATTRIBUTES 9
#define MAX_SPECIAL_ATTRIBUTES 3

class Card{
    public:
        Card();
        Card(string name, int cost);
        void buildCreature(int attack, int hp, string attackType = "Physical", string race = "Human");
        void buildSpell();
        void addTitle(string title);
        void addAttackImmunity(string immunity);
        void addStatusImmunity(string immunity);
        void addStatusAttribute(string status);
        string getName() const;
        string getTitle() const;
        int getAttack();
        int getHP();
        int getCost();
        void print() const;
        ~Card();
    private:
        int id;
        static int counter;
        int attack;
        int hp;
        // Cost for casting the card
        int cost;
        string name;
        string title; // title, complimenting the name of the card
        string race;
        string attackType; // Physical, Magical, Mythical
        bool isCreature;
        bool isSpell;
        bool hasSpawnEffect; // Effect that triggers when the card is casted
        bool attackImmunities[MAX_ATTACK_TYPES]; // Immunities to the several attack types
        /****************************************************************************************************************
         * Status Attributes
         * 1. Vampirism: Gain health equal to MIN(damage dealt, enemy hp remaining)
         * 2. Mana Harvest: Damage dealt is converted onto mana (MIN(damage dealt, enemy hp remaining))
         * 3. Mana Generation: Generate 1 extra mana per turn
         * 4. Berserk: After taking damage, increase attack by 1; after defeating an enemy, increase attack by 2
         * 5. First Strike: Attacks first in combat, therefore, if it defeats the enemy, the attacker will not be damaged
         * 6. Burn: Attacks add burn status to the enemy (deal 1 damage per turn)
         * 7. Freeze: Attacks add freeze status to the enemy (do not attack until get burned or randomly thawed)
         * 8. Paralysis: Attacks add paralysis status to the enemy (cannot attack back when being attacked)
         * 9. Stun: Attacks add stun status to the enemy (do not attack for 1 turn)
         ***************************************************************************************************************/
        bool statusAttributes[MAX_ATTRIBUTES];
        bool statusImmunities[MAX_ATTRIBUTES]; // Immunities to the aforementioned status attributes
        /****************************************************************************************************************
         * Special Status Attributes
         * 1. Poison: Attacks add poison status to the enemy (deal 1 damage for X turns, where X is this attr's value
         * 2. Decay: The target takes X damage every turn by decaying, where X is this attr's value
         * 3. Regeneration: Regenerates 1 health at the end of each turn
         ***************************************************************************************************************/
        int specialStatusAttributes[MAX_SPECIAL_ATTRIBUTES];
        bool specialStatusImmunities[MAX_SPECIAL_ATTRIBUTES]; // Immunities to the above special status attributes
};

std::map<string, Card> createDictionary();

#endif