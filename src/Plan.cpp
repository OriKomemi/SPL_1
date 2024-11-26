#include "Plan.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace std;

Plan::Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions)
    : plan_id(planId),
      settlement(settlement),
      selectionPolicy(selectionPolicy),
      status(PlanStatus::AVALIABLE),
      facilities(),
      underConstruction(),
      facilityOptions(facilityOptions),
      life_quality_score(0),
      economy_score(0),
      environment_score(0) {}


Plan::~Plan() {
    delete selectionPolicy;

    for (auto facility : facilities) {
        delete facility;
    }

    for (auto facility : underConstruction) {
        delete facility;
    }
}

Plan::Plan(const Plan &other)
    : plan_id(other.plan_id),
      settlement(other.settlement),
      selectionPolicy(other.selectionPolicy->clone()), // Clone policy
      status(other.status),
      facilities(),
      underConstruction(),
      facilityOptions(other.facilityOptions),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score) {
    for (const auto facility : other.facilities) {
        facilities.push_back(new Facility(*facility));
    }

    for (const auto facility : other.underConstruction) {
        underConstruction.push_back(new Facility(*facility));
    }
}

Plan &Plan::operator=(const Plan &other) {
    if (this == &other) return *this; // Handle self-assignment

    // Clean up current resources
    delete selectionPolicy;
    for (auto facility : facilities) delete facility;
    for (auto facility : underConstruction) delete facility;
    // Copy data
    plan_id = other.plan_id;
    selectionPolicy = other.selectionPolicy->clone(); // Clone policy
    status = other.status;
    // facilityOptions remains the same reference
    life_quality_score = other.life_quality_score;
    economy_score = other.economy_score;
    environment_score = other.environment_score;

    // Deep copy facilities
    facilities.clear();
    for (const auto facility : other.facilities) {
        facilities.push_back(new Facility(*facility));
    }

    underConstruction.clear();
    for (const auto facility : other.underConstruction) {
        underConstruction.push_back(new Facility(*facility));
    }

    return *this;
}


Plan::Plan(Plan &&other) noexcept
    : plan_id(other.plan_id),
      settlement(other.settlement),
      selectionPolicy(other.selectionPolicy),
      status(other.status),
      facilities(std::move(other.facilities)),
      underConstruction(std::move(other.underConstruction)),
      facilityOptions(other.facilityOptions),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score) {
    other.selectionPolicy = nullptr; // Nullify moved-from pointer
}


Plan &Plan::operator=(Plan &&other) noexcept {
    if (this == &other) return *this; // Handle self-assignment

    // Clean up current resources
    delete selectionPolicy;
    for (auto facility : facilities) delete facility;
    for (auto facility : underConstruction) delete facility;

    // Move data
    plan_id = other.plan_id;
    // settlement remains the same reference
    selectionPolicy = other.selectionPolicy;
    status = other.status;
    // facilityOptions remains the same reference
    facilities = std::move(other.facilities);
    underConstruction = std::move(other.underConstruction);
    life_quality_score = other.life_quality_score;
    economy_score = other.economy_score;
    environment_score = other.environment_score;

    other.selectionPolicy = nullptr; // Nullify moved-from pointer

    return *this;
}


// Getters for scores
const int Plan::getlifeQualityScore() const {
    return life_quality_score;
}

const int Plan::getEconomyScore() const {
    return economy_score;
}

const int Plan::getEnvironmentScore() const {
    return environment_score;
}

const int Plan::getPlanId() const {
    return plan_id;
}

// Set selection policy
void Plan::setSelectionPolicy(SelectionPolicy *newSelectionPolicy) {
    delete selectionPolicy; // Free old policy
    selectionPolicy = newSelectionPolicy;
}

// Simulate one step
void Plan::step() {
    // Step through under-construction facilities
    for (auto it = underConstruction.begin(); it != underConstruction.end();) {
        FacilityStatus status = (*it)->step();
        if (status == FacilityStatus::OPERATIONAL) {
            facilities.push_back(*it);
            it = underConstruction.erase(it); // Remove from under-construction
        } else {
            ++it;
        }
    }

    // Update scores
    life_quality_score = economy_score = environment_score = 0;
    for (const auto &facility : facilities) {
        life_quality_score += facility->getLifeQualityScore();
        economy_score += facility->getEconomyScore();
        environment_score += facility->getEnvironmentScore();
    }

    // Set status
    if (underConstruction.size() >= static_cast<size_t>(settlement.getType())) {
        status = PlanStatus::BUSY;
    } else {
        status = PlanStatus::AVALIABLE;
    }
}

// Print plan status
void Plan::printStatus() {
    cout << "Plan ID: " << plan_id << "\n"
         << "Settlement: " << settlement.getName() << "\n"
         << "Status: " << (status == PlanStatus::AVALIABLE ? "Available" : "Busy") << "\n"
         << "Scores:\n"
         << "  Life Quality: " << life_quality_score << "\n"
         << "  Economy: " << economy_score << "\n"
         << "  Environment: " << environment_score << "\n";

    cout << "Operational Facilities:\n";
    for (const auto &facility : facilities) {
        cout << "  - " << facility->toString() << "\n";
    }

    cout << "Under Construction Facilities:\n";
    for (const auto &facility : underConstruction) {
        cout << "  - " << facility->toString() << "\n";
    }
}

// Get facilities
const vector<Facility*> &Plan::getFacilities() const {
    return facilities;
}

// Add a facility to under-construction
void Plan::addFacility(Facility *facility) {
    if (underConstruction.size() < static_cast<size_t>(settlement.getType())) {
        underConstruction.push_back(facility);
    } else {
        throw runtime_error("Construction limit reached for plan ID: " + to_string(plan_id));
    }
}

// Convert to string
const string Plan::toString() const {
    return "Plan ID: " + to_string(plan_id) + ", Settlement: " + settlement.getName() + ", Status: " +
           (status == PlanStatus::AVALIABLE ? "Available" : "Busy");
}
