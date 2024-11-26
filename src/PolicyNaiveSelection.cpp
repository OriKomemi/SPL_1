#include "SelectionPolicy.h"
#include <stdexcept>

using namespace std;

// Constructor
NaiveSelection::NaiveSelection() : lastSelectedIndex(-1) {}

// Select facility
const FacilityType &NaiveSelection::selectFacility(const vector<FacilityType> &facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        throw runtime_error("No available facilities to select.");
    }
    lastSelectedIndex = (lastSelectedIndex + 1) % facilitiesOptions.size();
    return facilitiesOptions[lastSelectedIndex];
}

// Convert to string
const string NaiveSelection::toString() const {
    return "Naive Selection Policy";
}

// Clone policy
NaiveSelection *NaiveSelection::clone() const {
    return new NaiveSelection(*this);
}
