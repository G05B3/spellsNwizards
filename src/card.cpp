#include <iostream>
#include <string>
#include <map>

#include "card.h"

using namespace std;

enum attributes
{
    VAMPIRISM = 0,
    MANA_HARVEST,
    MANA_GENERATION,
    BERSERK,
    FIRST_STRIKE,
    BURN,
    FREEZE,
    PARALYSIS,
    STUN,
    UNKNOWN = -1
};
enum specialAttributes
{
    POISON = MAX_ATTRIBUTES,
    DECAY,
    REGENERATION,
    UNKOWN = -1
};
enum attackTypes
{
    PHYSICAL = 0,
    MAGICAL,
    MYTHICAL,
    NONE = -1
};

attributes attrHash(string attr)
{
    if (attr == "Vampirism")
        return VAMPIRISM;
    else if (attr == "Mana Harvest")
        return MANA_HARVEST;
    else if (attr == "Mana Generation")
        return MANA_GENERATION;
    else if (attr == "Berserk")
        return BERSERK;
    else if (attr == "First Strike")
        return FIRST_STRIKE;
    else if (attr == "Burn")
        return BURN;
    else if (attr == "Freeze")
        return FREEZE;
    else if (attr == "Paralysis")
        return PARALYSIS;
    else if (attr == "Stun")
        return STUN;
    return UNKNOWN; // Invalid attribute
}

string attrToString(int attr)
{
    switch (attr)
    {
    case VAMPIRISM:
        return "Vampirism";
    case MANA_HARVEST:
        return "Mana Harvest";
    case MANA_GENERATION:
        return "Mana Generation";
    case BERSERK:
        return "Berserk";
    case FIRST_STRIKE:
        return "First Strike";
    case BURN:
        return "Burn";
    case FREEZE:
        return "Freeze";
    case PARALYSIS:
        return "Paralysis";
    case STUN:
        return "Stun";
    default:
        return "Unknown";
    }
}

specialAttributes specialAttrHash(string attr)
{
    if (attr == "Poison")
        return POISON;
    else if (attr == "Decay")
        return DECAY;
    else if (attr == "Regeneration")
        return REGENERATION;
    return UNKOWN; // Invalid attribute
}

string specialAttrToString(int attr)
{
    switch (attr)
    {
    case POISON:
        return "Poison";
    case DECAY:
        return "Decay";
    case REGENERATION:
        return "Regeneration";
    default:
        return "Unkown";
    }
}

attackTypes attackTypeHash(string attackType)
{
    if (attackType == "Physical")
        return PHYSICAL;
    else if (attackType == "Magical")
        return MAGICAL;
    else if (attackType == "Mythical")
        return MYTHICAL;
    return NONE; // Invalid attack type
}

string attackTypeToString(int attackType)
{
    switch (attackType)
    {
    case PHYSICAL:
        return "Physical";
    case MAGICAL:
        return "Magical";
    case MYTHICAL:
        return "Mythical";
    default:
        return "None";
    }
}

int Card::counter = 1;

Card::Card()
: id(counter++), attack(1), hp(1), cost(1), name("NO NAME"), title("None"), race("None"), attackType("Physical"),
    isCreature(false), isSpell(false), hasSpawnEffect(false)
{
    for (int i = 0; i < MAX_ATTACK_TYPES; i++){
        attackImmunities[i] = false;
    }
    for (int i = 0; i < MAX_ATTRIBUTES; i++){
        statusAttributes[i] = false;
        statusImmunities[i] = false;
    }
    for (int i = 0; i < MAX_SPECIAL_ATTRIBUTES; i++){
        specialStatusAttributes[i] = 0;
        specialStatusImmunities[i] = false;
    }
}

// Constructor for empty card
Card::Card(string name, int cost)
: id(counter++), attack(1), hp(1), cost(cost), name(name), title("None"), race("None"), attackType("Physical"),
    isCreature(false), isSpell(false), hasSpawnEffect(false)
{
    for (int i = 0; i < MAX_ATTACK_TYPES; i++){
        attackImmunities[i] = false;
    }
    for (int i = 0; i < MAX_ATTRIBUTES; i++){
        statusAttributes[i] = false;
        statusImmunities[i] = false;
    }
    for (int i = 0; i < MAX_SPECIAL_ATTRIBUTES; i++){
        specialStatusAttributes[i] = 0;
        specialStatusImmunities[i] = false;
    }
}

void Card::buildCreature(int attack, int hp, string attackType, string race)
{
    this->attack = attack;
    this->race = race;
    this->hp = hp;
    this->attackType = attackType;
    this->isCreature = true;
}

void Card::buildSpell()
{
    this->isSpell = true;
    this->hasSpawnEffect = true;
}

void Card::addAttackImmunity(string immunity)
{
    int index = attackTypeHash(immunity);
    if (index >= 0 && index < MAX_ATTACK_TYPES)
        this->attackImmunities[index] = true;
}

void Card::addStatusImmunity(string immunity)
{
    int index = attrHash(immunity);
    if (index >= 0 && index < MAX_ATTRIBUTES)
        this->statusImmunities[index] = true;
    index = specialAttrHash(immunity);
    if (index >= MAX_ATTRIBUTES && index < MAX_SPECIAL_ATTRIBUTES + MAX_ATTRIBUTES)
        this->specialStatusImmunities[index - MAX_ATTRIBUTES] = true;
}

void Card::addStatusAttribute(string status)
{
    int index = attrHash(status);
    if (index >= 0 && index < MAX_ATTRIBUTES)
        this->statusAttributes[index] = true;
    index = specialAttrHash(status);
    if (index >= MAX_ATTRIBUTES && index < MAX_SPECIAL_ATTRIBUTES + MAX_ATTRIBUTES)
        this->specialStatusAttributes[index - MAX_ATTRIBUTES] = true;
}

void Card::addTitle(string title){
    this->title = title;
}

string Card::getName() const
{
    return this->name;
}

string Card::getTitle() const
{
    return this->title;
}

int Card::getAttack()
{
    return this->attack;
}

int Card::getHP()
{
    return this->hp;
}

int Card::getCost()
{
    return this->cost;
}

void Card::print() const
{
    cout << "----------------------------------------------------------" << endl;
    cout << "Card ID: " << id << endl;
    cout << name << (title == "None" ? "" : " , " + title) << endl;
    cout << "Attack: " << attack << ", HP: " << hp << ", Cost: " << cost << endl;
    cout << "Attack Type: " << attackType << endl;
    cout << "Creature? " << (isCreature ? race : "No") << " , Spell? " << (isSpell ? "Yes" : "No") << endl;
    cout << "\nAttack Immunities: " << endl;
    for (int i = 0; i < MAX_ATTACK_TYPES; i++)
    {
        if (attackImmunities[i])
            cout << "\t" << attackTypeToString(i) << endl;
    }    
    cout << "\nStatus Attributes: " << endl;
    for (int i = 0; i < MAX_ATTRIBUTES; i++)
    {
        if (statusAttributes[i])
            cout << "\t" << attrToString(i) << endl;
    }
    for (int i = MAX_ATTRIBUTES; i < MAX_SPECIAL_ATTRIBUTES + MAX_ATTRIBUTES; i++)
    {
        if (specialStatusAttributes[i - MAX_ATTRIBUTES] > 0)
            cout << "\t" << specialAttrToString(i) << " (" << specialStatusAttributes[i - MAX_ATTRIBUTES] << ")" << endl;
    }
    cout << "\nStatus Attribute Immunities: " << endl;
    for (int i = 0; i < MAX_ATTRIBUTES; i++)
    {
        if (statusImmunities[i])
            cout << "\t" << attrToString(i) << endl;
    }
    for (int i = MAX_ATTRIBUTES; i < MAX_ATTRIBUTES + MAX_SPECIAL_ATTRIBUTES; i++)
    {
        if (specialStatusImmunities[i - MAX_ATTRIBUTES] > 0)
            cout << "\t" << specialAttrToString(i) << endl;
    }

    cout << "Spawn Effect: " << (hasSpawnEffect ? "Yes" : "No") << endl;
    cout << "----------------------------------------------------------" << endl;
}

Card::~Card()
{
    // Currently no dynamic memory to free
}


map<string, Card> createDictionary()
{
    map<string, Card> cards;
    /**********************************
     * Creature Cards
     *********************************/
    
    /************ Humans *************/
    // Warrior
    cards.emplace("Warrior", Card("Warrior", 2));
    cards["Warrior"].buildCreature(2, 2);
    
    // Blue Mage
    cards.emplace("B. Mage", Card("B. Mage", 2));
    cards["B. Mage"].buildCreature(3, 1, "Magical");

    // Rogue
    cards.emplace("Rogue", Card("Rogue", 3));
    cards["Rogue"].buildCreature(2, 2);
    cards["Rogue"].addStatusAttribute("Poison");

    // Soldier
    cards.emplace("Soldier", Card("Soldier", 1));
    cards["Soldier"] = Card("Soldier", 1);
    cards["Soldier"].buildCreature(1, 1);

    // Knight
    cards.emplace("Knight", Card("Knight", 3));
    cards["Knight"].buildCreature(1, 3);
    cards["Knight"].addAttackImmunity("Magical");
    cards["Knight"].addStatusImmunity("Poison");

    // Pyromancer
    cards.emplace("Pyromancer", Card("Pyromancer", 4));
    cards["Pyromancer"].buildCreature(3, 2, "Magical");
    cards["Pyromancer"].addStatusAttribute("Burn");

    // Dark Knight
    cards.emplace("Dark Knight", Card("Dark Knight", 4));
    cards["Dark Knight"].buildCreature(1, 5);
    cards["Dark Knight"].addStatusAttribute("Berserk");


    /**********************************
     * Spells
     *********************************/

    // Nature's Blessing
    cards.emplace("Nature's Blessing", Card("Nature's Blessing", 2));
    cards["Nature's Blessing"].buildSpell();

    // Fireball
    cards.emplace("Fireball", Card("Fireball", 3));
    cards["Fireball"].buildSpell();
    cards["Fireball"].addStatusAttribute("Burn");

    // Lightning Bolt
    cards.emplace("Lightning Bolt", Card("Lightning Bolt", 1));
    cards["Lightning Bolt"].buildSpell(); // Deals 2 damage + paralysis
    cards["Lightning Bolt"].addStatusAttribute("Paralysis");

    return cards;
}