#include "SelectionPolicy.h"
#include <limits>
#include <stdexcept>

using namespace std;


BalancedSelection::BalancedSelection(int LifeQualityScore, int EconomyScore, int EnvironmentScore)
    : LifeQualityScore(LifeQualityScore), EconomyScore(EconomyScore), EnvironmentScore(EnvironmentScore) {}

const FacilityType &BalancedSelection::selectFacility(const vector<FacilityType> &facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        throw runtime_error("No available facilities to select.");
    }

    int minDifference = numeric_limits<int>::max();
    const FacilityType *selected = nullptr;

    for (const auto &facility : facilitiesOptions) {
        int scoreA = LifeQualityScore + facility.getLifeQualityScore();
        int scoreB = EconomyScore + facility.getEconomyScore();
        int scoreC = EnvironmentScore + facility.getEnvironmentScore();
        int difference = max(abs(scoreA - scoreB), abs(scoreB - scoreC));
        difference = max(difference, abs(scoreC - scoreA));
        if (difference < minDifference) {
            minDifference = difference;
            selected = &facility;
        }
    }

    if (!selected) {
        throw runtime_error("Failed to select facility.");
    }

    return *selected;
}

const string BalancedSelection::toString() const {
    return "Balanced Selection Policy";
}

BalancedSelection *BalancedSelection::clone() const {
    return new BalancedSelection(*this);
}
