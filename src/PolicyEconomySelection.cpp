#include "SelectionPolicy.h"
#include <stdexcept>
#include <iostream>

using namespace std;

// Constructor
EconomySelection::EconomySelection() : lastSelectedIndex(-1) {}

// Select facility
const FacilityType &EconomySelection::selectFacility(const vector<FacilityType> &facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        cout << "No available facilities to select." << endl;
    }

    for (size_t i = 0; i < facilitiesOptions.size(); ++i) {
        size_t index = (lastSelectedIndex + 1 + i) % facilitiesOptions.size();
        if (facilitiesOptions[index].getCategory() == FacilityCategory::ECONOMY) {
            lastSelectedIndex = index;
            return facilitiesOptions[index];
        }
    }

    cout << "No economy-related facilities available." << endl;
}

// Convert to string
const string EconomySelection::toString() const {
    return "Economy Selection Policy last selected index " + to_string(lastSelectedIndex);
}

// Clone policy
EconomySelection *EconomySelection::clone() const {
    return new EconomySelection(*this);
}
