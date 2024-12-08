#include "Plan.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace std;

Plan::Plan(const int planId,
           const Settlement &settlement,
           SelectionPolicy *selectionPolicy,
           const std::vector<FacilityType> &facilityOptions,
           int life_quality_score,
           int economy_score,
           int environment_score,
           std::vector<Facility *> facilities,
           std::vector<Facility *> underConstruction)
    : Plan(planId, settlement, selectionPolicy, facilityOptions) // Delegate to first constructor
{
    // Additional initialization
    this->life_quality_score = life_quality_score;
    this->economy_score = economy_score;
    this->environment_score = environment_score;
    this->facilities = std::move(facilities);
    this->underConstruction = std::move(underConstruction);
}

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

Plan::~Plan()
{
    for (auto f : facilities)
    {
        delete f;
    }
    facilities.clear();
    for (auto f : underConstruction)
    {
        delete f;
    }
    underConstruction.clear();
    if (selectionPolicy)
    {
        delete selectionPolicy;
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
      environment_score(other.environment_score)
{
    for (const auto facility : other.facilities)
    {
        facilities.push_back(new Facility(*facility));
    }

    for (const auto facility : other.underConstruction)
    {
        underConstruction.push_back(new Facility(*facility));
    }
}

Plan &Plan::operator=(const Plan &other)
{
    if (this == &other)
    {
        return *this;
    }

    // Clean up existing resources
    for (auto f : facilities)
    {
        delete f;
    }
    facilities.clear();

    for (auto f : underConstruction)
    {
        delete f;
    }
    underConstruction.clear();

    delete selectionPolicy;
    selectionPolicy = nullptr;

    plan_id = other.plan_id;
    // settlement = other.settlement; // Not possible if it's a reference and must remain from constructor
    // settlement is a reference, so we handle appropriately with cloneDeep function when relevant
    life_quality_score = other.life_quality_score;
    economy_score = other.economy_score;
    environment_score = other.environment_score;
    selectionPolicy = other.selectionPolicy ? other.selectionPolicy->clone() : nullptr;

    // Deep copy facilities
    for (auto f : other.facilities)
    {
        facilities.push_back(f->clone());
    }

    // Deep copy under construction
    for (auto f : other.underConstruction)
    {
        underConstruction.push_back(f->clone());
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
      facilityOptions(std::move(other.facilityOptions)),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score)
{
    other.selectionPolicy = nullptr; // Nullify moved-from pointer
}

Plan Plan::cloneDeep(const std::vector<Settlement *> &settlements,
                     const std::vector<FacilityType> &facilitiesOptions) const
{
    // Find corresponding Settlement by name
    Settlement *newSettlement = nullptr;
    for (Settlement *currentSettlement : settlements)
    {
        if (currentSettlement->getName() == settlement.getName())
        {
            newSettlement = currentSettlement;
            break;
        }
    }

    // Ensure newSettlement is valid
    if (!newSettlement)
    {
        throw std::runtime_error("Settlement not found for cloning Plan");
    }

    // Deep copy facilities
    std::vector<Facility *> newFacilities;
    newFacilities.reserve(facilities.size());
    for (Facility *facility : facilities)
    {
        newFacilities.push_back(facility->clone());
    }

    // Deep copy under-construction facilities
    std::vector<Facility *> newUnderConstruction;
    newUnderConstruction.reserve(underConstruction.size());
    for (Facility *facility : underConstruction)
    {
        newUnderConstruction.push_back(facility->clone());
    }

    // Clone selectionPolicy if exists
    SelectionPolicy *newPolicy = selectionPolicy ? selectionPolicy->clone() : nullptr;

    // Create and return the new Plan object
    return Plan(
        plan_id,
        *newSettlement,
        newPolicy,
        facilitiesOptions,
        life_quality_score,
        economy_score,
        environment_score,
        std::move(newFacilities),
        std::move(newUnderConstruction));
}

// Getters for scores
const int Plan::getlifeQualityScore() const
{
    return life_quality_score;
}

const int Plan::getEconomyScore() const
{
    return economy_score;
}

const int Plan::getEnvironmentScore() const
{
    return environment_score;
}

const int Plan::getPlanId() const
{
    return plan_id;
}

PlanStatus Plan::getPlanStatus()
{
    if ((int)underConstruction.size() < getConstructionLimit())
    {
        status = PlanStatus::AVALIABLE;
    }
    else
    {
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
void Plan::setSelectionPolicy(SelectionPolicy *newSelectionPolicy)
{
    delete selectionPolicy; // Free old policy
    selectionPolicy = newSelectionPolicy;
}

// Simulate one step
void Plan::step()
{
    while (getPlanStatus() == PlanStatus::AVALIABLE)
    {
        addFacility(new Facility(selectionPolicy->selectFacility(facilityOptions), settlement.getName()));
    }

    for (auto facility = underConstruction.begin(); facility != underConstruction.end();)
    {
        FacilityStatus status = (*facility)->step();
        if (status == FacilityStatus::OPERATIONAL)
        {
            facilities.push_back(*facility);
            facility = underConstruction.erase(facility); // Remove from under-construction
        }
        else
        {
            ++facility;
        }
    }

    // Update scores
    life_quality_score = economy_score = environment_score = 0;
    for (Facility *facility : facilities)
    {
        life_quality_score += facility->getlifeQualityScore();
        economy_score += facility->getEconomyScore();
        environment_score += facility->getEnvironmentScore();
    }
    // Set status
    if ((int)underConstruction.size() < getConstructionLimit())
    {
        status = PlanStatus::AVALIABLE;
    }
    else
    {
        status = PlanStatus::BUSY;
    }
}

// Print plan status
void Plan::printStatus()
{
    std::cout << "PlanID: " << plan_id << std::endl;
    std::cout << "SettlementName: " << settlement.getName() << std::endl;
    std::cout << "PlanStatus: " << (status == PlanStatus::BUSY ? "BUSY" : "AVAILABLE") << std::endl;
    std::cout << "SelectionPolicy: " << selectionPolicy->toString() << std::endl;
    std::cout << "LifeQualityScore: " << life_quality_score << std::endl;
    std::cout << "EconomyScore: " << economy_score << std::endl;
    std::cout << "EnvironmentScore: " << environment_score << std::endl;
    // Using range-based for loop to iterate over facilities vector
    for (const auto &facility : facilities)
    {
        if (facility != nullptr)
        {
            std::cout << "FacilityName: " << facility->getName() << std::endl;
            std::cout << "FacilityStatus: OPERATIONAL" << std::endl;
        }
    }

    // Using range-based for loop to iterate over underConstruction vector
    for (const auto &facility : underConstruction)
    {
        if (facility != nullptr)
        {
            std::cout << "FacilityName: " << facility->getName() << std::endl;
            std::cout << "FacilityStatus: UNDER_CONSTRUCTION" << std::endl;
        }
    }
}

void Plan::printShortStatus()
{
    std::cout << "PlanID: " << plan_id << std::endl;
    std::cout << "SettlementName: " << settlement.getName() << std::endl;
    std::cout << "LifeQualityScore: " << life_quality_score << std::endl;
    std::cout << "EconomyScore: " << economy_score << std::endl;
    std::cout << "EnvironmentScore: " << environment_score << std::endl;
}

// Get facilities
const vector<Facility *> &Plan::getFacilities() const
{
    return facilities;
}

// Add a facility to under-construction
void Plan::addFacility(Facility *facility)
{
    if ((int)underConstruction.size() < getConstructionLimit())
    {
        underConstruction.push_back(facility);
    }
    else
    {
        cout << "Construction limit reached for plan ID: " + to_string(plan_id) << endl;
    }
}

// Convert to string
const string Plan::toString() const
{
    return "Plan ID: " + to_string(plan_id) + ", Settlement: " + settlement.getName() + ", Status: " +
           (status == PlanStatus::AVALIABLE ? "Available" : "Busy");
}
