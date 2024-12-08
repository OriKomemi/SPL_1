#include "Plan.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace std;

Plan::Plan(int id,
           const Settlement &settlement,
           SelectionPolicy *policy,
           const std::vector<FacilityType> &facilityOptions,
           int lifeQuality,
           int economy,
           int environment,
           std::vector<Facility*> facilities,
           std::vector<Facility*> underConstruction)
    : plan_id(id),
      settlement(settlement),
      selectionPolicy(policy),
      facilityOptions(facilityOptions),
      life_quality_score(lifeQuality),
      economy_score(economy),
      environment_score(environment),
      facilities(std::move(facilities)),
      underConstruction(std::move(underConstruction)),
      status(PlanStatus::AVALIABLE) {}

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
    for (Facility* facility : facilities) {
        delete facility;
    }

    for (Facility* facility : underConstruction) {
        delete facility;
    }

    delete selectionPolicy;
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


Plan::Plan(Plan &&other) noexcept
    : plan_id(other.plan_id),
      settlement(other.settlement),
      selectionPolicy(other.selectionPolicy),
      status(other.status),
      facilities(std::move(other.facilities)),
      underConstruction(std::move(other.underConstruction)),
      facilityOptions(std::move(other.facilityOptions)),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score) {
    other.selectionPolicy = nullptr; // Nullify moved-from pointer
}

Plan Plan::cloneDeep(const std::vector<Settlement *> &settlements, const std::vector<FacilityType> &facilitiesOptions) const {
    // Find corresponding Settlement
    Settlement *newSettlement = nullptr;
    for (Settlement *copiedSettlement : settlements) {
        if (copiedSettlement->getName() == settlement.getName()) {
            newSettlement = copiedSettlement;
            break;
        }
    }

    // Ensure newSettlement is valid
    if (!newSettlement) {
        throw std::runtime_error("Settlement not found for cloning Plan");
    }

    // Deep copy facilities
    std::vector<Facility *> copiedFacilities;
    for (Facility *facility : facilities) {
        copiedFacilities.push_back(facility->clone());
    }

    // Deep copy under-construction facilities
    std::vector<Facility *> copiedUnderConstruction;
    for (Facility *facility : underConstruction) {
        copiedUnderConstruction.push_back(facility->clone());
    }

    // Create and return the new Plan object
    return Plan(
        plan_id,
        *newSettlement,
        selectionPolicy ? selectionPolicy->clone() : nullptr,
        facilitiesOptions,
        life_quality_score,
        economy_score,
        environment_score,
        std::move(copiedFacilities),
        std::move(copiedUnderConstruction)
    );
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
        life_quality_score += facility->getlifeQualityScore();
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
    // Using range-based for loop to iterate over facilities vector
    for (const auto& facility : facilities) {
        if (facility != nullptr) {
            std::cout << "FacilityName: " << facility->getName() << std::endl;
            std::cout << "FacilityStatus: OPERATIONAL" << std::endl;
        }
    }

    // Using range-based for loop to iterate over underConstruction vector
    for (const auto& facility : underConstruction) {
        if (facility != nullptr) {
            std::cout << "FacilityName: " << facility->getName() << std::endl;
            std::cout << "FacilityStatus: UNDER_CONSTRUCTION" << std::endl;
        }
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
