#include "SelectionPolicy.h"
#include <stdexcept>
#include <iostream>

using namespace std;

// Constructor
SustainabilitySelection::SustainabilitySelection() : lastSelectedIndex(-1) {}

// Select facility
const FacilityType &SustainabilitySelection::selectFacility(const vector<FacilityType> &facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        cout << "No available facilities to select." << endl;
    }

    for (size_t i = 0; i < facilitiesOptions.size(); ++i) {
        size_t index = (lastSelectedIndex + 1 + i) % facilitiesOptions.size();
        if (facilitiesOptions[index].getCategory() == FacilityCategory::ENVIRONMENT) {
            lastSelectedIndex = index;
            return facilitiesOptions[index];
        }
    }

    throw std::runtime_error("No suitable facility found");
}

// Convert to string
const string SustainabilitySelection::toString() const {
    return "env";
}

const string SustainabilitySelection::getPolicyType() const
{
    return "env";
}

// Clone policy
SustainabilitySelection *SustainabilitySelection::clone() const {
    return new SustainabilitySelection(*this);
}
