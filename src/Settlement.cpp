#include "Settlement.h"

// Constructor Implementation
Settlement::Settlement(const string &name, SettlementType type)
    : name(name), type(type) {}

// Getter for the name
const string &Settlement::getName() const
{
    return name;
}

// Getter for the settlement type
SettlementType Settlement::getType() const
{
    return type;
}

// Convert settlement details to string
const string Settlement::toString() const
{
    string typeString;
    switch (type)
    {
    case SettlementType::VILLAGE:
        typeString = "Village";
        break;
    case SettlementType::CITY:
        typeString = "City";
        break;
    case SettlementType::METROPOLIS:
        typeString = "Metropolis";
        break;
    }
    return "Settlement Name: " + name + ", Type: " + typeString;
}
