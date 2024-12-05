#include "Action.h"
#include <string>
#include <iostream>


BaseAction::BaseAction() : status(ActionStatus::ERROR), errorMsg("") {}

void BaseAction::complete()
{
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg)
{
    errorMsg = std::move(errorMsg);
    status = ActionStatus::ERROR;
}

const string &BaseAction::getErrorMsg() const
{
    return errorMsg;
}

ActionStatus BaseAction::getStatus() const {
    return status;
}

SimulateStep::SimulateStep(const int numOfSteps): numOfSteps(numOfSteps) {}

void SimulateStep::act(Simulation &simulation)
{
    for(int i=0; i<numOfSteps; i++){
        simulation.step();
    }
    complete();
}

const string SimulateStep::toString() const {
    if (getStatus() == ActionStatus::COMPLETED) {
        return "step " + std::to_string(numOfSteps) + " COMPLETED";
    } else {
        return "step " + std::to_string(numOfSteps) + " ERROR: " + getErrorMsg();
    }
}

SimulateStep *SimulateStep::clone() const
{
    return new SimulateStep(numOfSteps);
}


AddPlan::AddPlan(const string &settlementName, const string &selectionPolicy): settlementName(settlementName), selectionPolicy(selectionPolicy) {}

void AddPlan::act(Simulation &simulation)
{
        if (simulation.isSettlementExists(settlementName)){
            Settlement &settlement = simulation.getSettlement(settlementName);
            SelectionPolicy *policy = nullptr;
            if (selectionPolicy == "nve") {
                policy = new NaiveSelection();
            } else if (selectionPolicy == "bal") {
                policy = new BalancedSelection(0, 0, 0);
            } else if (selectionPolicy == "eco") {
                policy = new EconomySelection();
            } else if (selectionPolicy == "env") {
                policy = new SustainabilitySelection();
            } else {
                error("Cannot create this plan");
            }
            simulation.addPlan(settlement, policy);
            complete();
        } else {
            error("Cannot create this plan");
        }

}

const string AddPlan::toString() const
{
    // TODO: wirte massage
    return "AddPlan";
}

AddPlan *AddPlan::clone() const
{
    return new AddPlan(*this);
}

AddSettlement::AddSettlement(const string &settlementName, SettlementType settlementType): settlementName(settlementName), settlementType(settlementType) {}

void AddSettlement::act(Simulation &simulation) {
    Settlement *settlement = new Settlement(settlementName, settlementType);
    if (!simulation.addSettlement(settlement)) {
        delete settlement;
        error("Settlement already exists");
    } else {
        complete();
    }
}

AddSettlement *AddSettlement::clone() const {
    return new AddSettlement(*this);
}

const string AddSettlement::toString() const
{
    return string();
}

AddFacility::AddFacility(const string &facilityName, const FacilityCategory facilityCategory, const int price, const int lifeQualityScore, const int economyScore, const int environmentScore):
facilityName(facilityName), facilityCategory(facilityCategory), price(price), lifeQualityScore(lifeQualityScore), economyScore(economyScore), environmentScore(environmentScore) {}

void AddFacility::act(Simulation &simulation)
{
    FacilityType facility(facilityName, facilityCategory, price, lifeQualityScore, economyScore, environmentScore);
    if (!simulation.addFacility(facility)) {
        error("Facility alreadyexists");
    } else {
        complete();
    }
}

AddFacility *AddFacility::clone() const
{
    return new AddFacility(*this);
}

const string AddFacility::toString() const
{
    return string();
}

PrintPlanStatus::PrintPlanStatus(int planId): planId(planId) {}

void PrintPlanStatus::act(Simulation &simulation)
{

    Plan plan = simulation.getPlan(planId);
    plan.printStatus();
    complete();
}

BackupSimulation::BackupSimulation() {}

void BackupSimulation::act(Simulation &simulation) {
    if (backup != nullptr) {
        delete backup;
    }
    backup = simulation.clone();
    complete();
}

BackupSimulation* BackupSimulation::clone() const {
    return new BackupSimulation(*this);
}

const string BackupSimulation::toString() const {
    return "backup COMPLETED";
}

RestoreSimulation::RestoreSimulation() {}

void RestoreSimulation::act(Simulation &simulation) {
    if (backup == nullptr) {
        error("No backup available");
        return;
    }

    simulation = *backup;
    complete();
}

RestoreSimulation* RestoreSimulation::clone() const {
    return new RestoreSimulation(*this);
}

const std::string RestoreSimulation::toString() const {
    if (getStatus() == ActionStatus::COMPLETED) {
        return "restore COMPLETED";
    } else {
        return "restore ERROR: " + getErrorMsg();
    }
}
