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
const int Plan::getLifeQualityScore() const {
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

PlanStatus Plan::getPlanStatus() {
    if (underConstruction.size() < getConstructionLimit()) {
        status = PlanStatus::AVALIABLE;
    } else {
        status = PlanStatus::BUSY;
    }
    return status;
}

const string Plan::getSelectionPolicyType()
{
    return this->selectionPolicy->getPolicyType();
}

const int Plan::getConstructionLimit()
{
    return static_cast<int>(settlement.getType()) + 1;
}

// Set selection policy
void Plan::setSelectionPolicy(SelectionPolicy *newSelectionPolicy) {
    if (selectionPolicy->toString()==newSelectionPolicy->toString()) {
        delete newSelectionPolicy;
        std::cout << "Cannot change selection policy" << std::endl;
    }
    std::cout << "planID: " << plan_id << std::endl;
    std::cout << "previousPolicy: " << selectionPolicy->toString() << std::endl;
    delete selectionPolicy; // Free old policy
    selectionPolicy = newSelectionPolicy;
    std::cout << "newPolicy: " << selectionPolicy->toString() << std::endl;
}

// Simulate one step
void Plan::step() {
    while(getPlanStatus() == PlanStatus::AVALIABLE) {
        addFacility(new Facility(selectionPolicy->selectFacility(facilityOptions), settlement.getName()));
    }

    for (auto facility = underConstruction.begin(); facility != underConstruction.end();) {
        FacilityStatus status = (*facility)->step();
        if (status == FacilityStatus::OPERATIONAL) {
            facilities.push_back(*facility);
            facility = underConstruction.erase(facility); // Remove from under-construction
        } else {
            ++facility;
        }
    }

    // Update scores
    life_quality_score = economy_score = environment_score = 0;
    for (Facility *facility : facilities) {
        life_quality_score += facility->getLifeQualityScore();
        economy_score += facility->getEconomyScore();
        environment_score += facility->getEnvironmentScore();
    }
    // Set status
    if (underConstruction.size() < getConstructionLimit()) {
        status = PlanStatus::AVALIABLE;
    } else {
        status = PlanStatus::BUSY;
    }
}

// Print plan status
void Plan::printStatus() {
    std::cout << "PlanID: " << plan_id << std::endl;
    std::cout << "SettlementName: " << settlement.getName() << std::endl;
    std::cout << "PlanStatus: " << (status == PlanStatus::BUSY ? "BUSY" : "AVAILABLE") << std::endl;
    std::cout << "SelectionPolicy: " << selectionPolicy->toString() << std::endl;
    std::cout << "LifeQualityScore: " << life_quality_score << std::endl;
    std::cout << "EconomyScore: " << economy_score << std::endl;
    std::cout << "EnvironmentScore: " << environment_score << std::endl;
    for (int i = 0; i < facilities.size(); ++i) {
        std::cout << "FacilityName: " << facilities[i]->getName() << std::endl;
        std::cout << "FacilityStatus: OPERATIONAL" << std::endl;
    }
    for (int i = 0; i < underConstruction.size(); ++i) {
        std::cout << "FacilityName: " << facilities[i]->getName() << std::endl;
        std::cout << "FacilityStatus: UNDERCONSTRUCTIONS" << std::endl;
    }
}
void Plan::printShortStatus() {
    std::cout << "PlanID: " << plan_id << std::endl;
    std::cout << "SettlementName: " << settlement.getName() << std::endl;
    std::cout << "LifeQualityScore: " << life_quality_score << std::endl;
    std::cout << "EconomyScore: " << economy_score << std::endl;
    std::cout << "EnvironmentScore: " << environment_score << std::endl;
}


// Get facilities
const vector<Facility*> &Plan::getFacilities() const {
    return facilities;
}

// Add a facility to under-construction
void Plan::addFacility(Facility *facility) {
    if (underConstruction.size() < getConstructionLimit()) {
        underConstruction.push_back(facility);
    } else {
        cout << "Construction limit reached for plan ID: " + to_string(plan_id) << endl;
    }
}


// Convert to string
const string Plan::toString() const {
    return "Plan ID: " + to_string(plan_id) + ", Settlement: " + settlement.getName() + ", Status: " +
           (status == PlanStatus::AVALIABLE ? "Available" : "Busy");
}
