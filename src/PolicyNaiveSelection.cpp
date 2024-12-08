#include "SelectionPolicy.h"
#include <stdexcept>
#include <iostream>

using namespace std;

// Constructor
NaiveSelection::NaiveSelection() : lastSelectedIndex(-1) {}

// Select facility
const FacilityType &NaiveSelection::selectFacility(const vector<FacilityType> &facilitiesOptions)
{
    if (facilitiesOptions.empty())
    {
        cout << "No available facilities to select." << endl;
    }
    lastSelectedIndex = (lastSelectedIndex + 1) % facilitiesOptions.size();
    return facilitiesOptions[lastSelectedIndex];
}

// Convert to string
const string NaiveSelection::toString() const
{
    return "nve";
}

const string NaiveSelection::getPolicyType() const
{
    return "nav";
}
// Clone policy
NaiveSelection *NaiveSelection::clone() const
{
    return new NaiveSelection(*this);
}
