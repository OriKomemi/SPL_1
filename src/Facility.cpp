// Facility.cpp
#include "Facility.h"

Facility::Facility(const string &name, const string &settlementName, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score)
    : FacilityType(name, category, price, lifeQuality_score, economy_score, environment_score),
      settlementName(settlementName),
      status(FacilityStatus::UNDER_CONSTRUCTIONS),
      timeLeft(price) {}

Facility::Facility(const FacilityType &type, const string &settlementName)
    : FacilityType(type),
      settlementName(settlementName),
      status(FacilityStatus::UNDER_CONSTRUCTIONS),
      timeLeft(type.getCost()) {}

const string &Facility::getSettlementName() const
{
    return settlementName;
}

const int Facility::getTimeLeft() const
{
    return timeLeft;
}

FacilityStatus Facility::step()
{
    if (timeLeft > 0)
    {
        --timeLeft;
    }
    if (timeLeft == 0)
    {
        status = FacilityStatus::OPERATIONAL;
    }
    return status;
}

Facility *Facility::clone() const
{
    return new Facility(*this);
}

void Facility::setStatus(FacilityStatus status)
{
    this->status = status;
}

const FacilityStatus &Facility::getStatus() const
{
    return status;
}

const string Facility::toString() const
{
    string statusStr = (status == FacilityStatus::UNDER_CONSTRUCTIONS) ? "UNDER_CONSTRUCTIONS" : "OPERATIONAL";
    return "Facility: " + getName() + ", Settlement: " + settlementName + ", Status: " + statusStr + ", Time Left: " + std::to_string(timeLeft);
    ;
}
