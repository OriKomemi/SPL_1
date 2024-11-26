#include "SelectionPolicy.h"
#include <stdexcept>

using namespace std;

// Constructor
EconomySelection::EconomySelection() : lastSelectedIndex(-1) {}

// Select facility
const FacilityType &EconomySelection::selectFacility(const vector<FacilityType> &facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        throw runtime_error("No available facilities to select.");
    }

    for (size_t i = 0; i < facilitiesOptions.size(); ++i) {
        size_t index = (lastSelectedIndex + 1 + i) % facilitiesOptions.size();
        if (facilitiesOptions[index].getCategory() == FacilityCategory::ECONOMY) {
            lastSelectedIndex = index;
            return facilitiesOptions[index];
        }
    }

    throw runtime_error("No economy-related facilities available.");
}

// Convert to string
const string EconomySelection::toString() const {
    return "Economy Selection Policy";
}

// Clone policy
EconomySelection *EconomySelection::clone() const {
    return new EconomySelection(*this);
}
