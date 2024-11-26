#include "SelectionPolicy.h"
#include <stdexcept>

using namespace std;

// Constructor
SustainabilitySelection::SustainabilitySelection() : lastSelectedIndex(-1) {}

// Select facility
const FacilityType &SustainabilitySelection::selectFacility(const vector<FacilityType> &facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        throw runtime_error("No available facilities to select.");
    }

    for (size_t i = 0; i < facilitiesOptions.size(); ++i) {
        size_t index = (lastSelectedIndex + 1 + i) % facilitiesOptions.size();
        if (facilitiesOptions[index].getCategory() == FacilityCategory::ENVIRONMENT) {
            lastSelectedIndex = index;
            return facilitiesOptions[index];
        }
    }

    throw runtime_error("No sustainability-related facilities available.");
}

// Convert to string
const string SustainabilitySelection::toString() const {
    return "Sustainability Selection Policy";
}

// Clone policy
SustainabilitySelection *SustainabilitySelection::clone() const {
    return new SustainabilitySelection(*this);
}
