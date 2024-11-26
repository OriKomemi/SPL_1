#include "Facility.h"

// Constructor: Create Facility from scratch
Facility::Facility(const string &name, const string &settlementName, const FacilityCategory category, const int price,
                   const int lifeQuality_score, const int economy_score, const int environment_score)
    : FacilityType(name, category, price, lifeQuality_score, economy_score, environment_score),
      settlementName(settlementName), status(FacilityStatus::UNDER_CONSTRUCTIONS), timeLeft(price) {}

// Constructor: Create Facility from FacilityType
Facility::Facility(const FacilityType &type, const string &settlementName)
    : FacilityType(type), settlementName(settlementName), status(FacilityStatus::UNDER_CONSTRUCTIONS), timeLeft(type.getCost()) {}

// Getter for settlement name
const string &Facility::getSettlementName() const {
    return settlementName;
}

// Getter for time left
const int Facility::getTimeLeft() const {
    return timeLeft;
}

// Perform a simulation step
FacilityStatus Facility::step() {
    if (timeLeft > 0) {
        --timeLeft; // Decrement construction time
    }
    if (timeLeft == 0) {
        status = FacilityStatus::OPERATIONAL;
    }
    return status;
}

// Set facility status
void Facility::setStatus(FacilityStatus newStatus) {
    status = newStatus;
}

// Get current facility status
const FacilityStatus &Facility::getStatus() const {
    return status;
}

// Convert facility details to string
const string Facility::toString() const {
    return "Facility: " + name + ", Settlement: " + settlementName +
           ", Status: " + (status == FacilityStatus::UNDER_CONSTRUCTIONS ? "Under Construction" : "Operational") +
           ", Time Left: " + std::to_string(timeLeft);
}
