#pragma once
#include <vector>
#include "Facility.h"
#include "Settlement.h"
#include "SelectionPolicy.h"
using std::vector;

enum class PlanStatus
{
    AVALIABLE,
    BUSY,
};

class Plan
{
public:
    Plan(int id,
         const Settlement &settlement,
         SelectionPolicy *policy,
         const std::vector<FacilityType> &facilityOptions,
         int lifeQuality,
         int economy,
         int environment,
         std::vector<Facility *> facilities,
         std::vector<Facility *> underConstruction);
    Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions);
    // Rule of 5
    ~Plan();                            // Destructor
    Plan(const Plan &other);            // Copy Constructor
    Plan &operator=(const Plan &other); // Copy operator
    Plan(Plan &&other) noexcept;        // Move Constructor

    Plan cloneDeep(const std::vector<Settlement *> &settlements, const std::vector<FacilityType> &facilitiesOptions) const;

    const int getlifeQualityScore() const;
    const int getEconomyScore() const;
    const int getEnvironmentScore() const;
    const int getPlanId() const;
    const int getConstructionLimit();
    PlanStatus getPlanStatus();
    const string getSelectionPolicyType();

    void setSelectionPolicy(SelectionPolicy *selectionPolicy);
    void step();
    void printStatus();
    void printShortStatus();
    const vector<Facility *> &getFacilities() const;
    void addFacility(Facility *facility);
    const string toString() const;

private:
    int plan_id;
    const Settlement &settlement;
    SelectionPolicy *selectionPolicy; // What happens if we change this to a reference?
    PlanStatus status;
    vector<Facility *> facilities;
    vector<Facility *> underConstruction;
    const vector<FacilityType> &facilityOptions;
    int life_quality_score, economy_score, environment_score;
};