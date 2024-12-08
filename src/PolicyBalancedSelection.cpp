#include "SelectionPolicy.h"
#include <limits>
#include <stdexcept>
#include <iostream>

using namespace std;

BalancedSelection::BalancedSelection(int LifeQualityScore, int EconomyScore, int EnvironmentScore)
    : LifeQualityScore(LifeQualityScore), EconomyScore(EconomyScore), EnvironmentScore(EnvironmentScore) {}

const FacilityType &BalancedSelection::selectFacility(const vector<FacilityType> &facilitiesOptions)
{
    if (facilitiesOptions.empty())
    {
        cout << "No available facilities to select." << endl;
    }

    int minDifference = numeric_limits<int>::max();
    const FacilityType *selected = nullptr;

    for (const auto &facility : facilitiesOptions)
    {
        int scoreA = LifeQualityScore + facility.getlifeQualityScore();
        int scoreB = EconomyScore + facility.getEconomyScore();
        int scoreC = EnvironmentScore + facility.getEnvironmentScore();
        int difference = max(abs(scoreA - scoreB), abs(scoreB - scoreC));
        difference = max(difference, abs(scoreC - scoreA));
        if (difference < minDifference)
        {
            minDifference = difference;
            selected = &facility;
        }
    }

    if (!selected)
    {
        cout << "Failed to select facility." << endl;
    }
    else
    {
        LifeQualityScore += selected->getlifeQualityScore();
        EconomyScore += selected->getEconomyScore();
        EnvironmentScore += selected->getEnvironmentScore();
    }

    return *selected;
}

const string BalancedSelection::toString() const
{
    return "bal";
}

const string BalancedSelection::getPolicyType() const
{
    return "bal";
}

BalancedSelection *BalancedSelection::clone() const
{
    return new BalancedSelection(*this);
}
