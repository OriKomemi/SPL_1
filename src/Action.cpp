#include "Action.h"
#include "backup.h"
#include "SelectionPolicy.h"
#include "Plan.h"
#include <string>
#include <iostream>
#include <iostream>

BaseAction::BaseAction() : errorMsg(""), status(ActionStatus::ERROR) {}

void BaseAction::complete()
{
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(string msg)
{
    errorMsg = std::move(msg);
    status = ActionStatus::ERROR;
    std::cout << "Error: " + errorMsg << std::endl;
}

const string &BaseAction::getErrorMsg() const
{
    return errorMsg;
}

ActionStatus BaseAction::getStatus() const
{
    return status;
}

SimulateStep::SimulateStep(const int numOfSteps) : numOfSteps(numOfSteps) {}

void SimulateStep::act(Simulation &simulation)
{
    for (int i = 0; i < numOfSteps; i++)
    {
        simulation.step();
    }
    complete();
}

const string SimulateStep::toString() const
{
    if (getStatus() == ActionStatus::COMPLETED)
    {
        return "step " + std::to_string(numOfSteps) + " COMPLETED";
    }
    else
    {
        return "step " + std::to_string(numOfSteps) + " ERROR: " + getErrorMsg();
    }
}

SimulateStep *SimulateStep::clone() const
{
    return new SimulateStep(numOfSteps);
}

AddPlan::AddPlan(const string &settlementName, const string &selectionPolicy) : settlementName(settlementName), selectionPolicy(selectionPolicy) {}

void AddPlan::act(Simulation &simulation)
{
    if (simulation.isSettlementExists(settlementName))
    {
        Settlement &settlement = simulation.getSettlement(settlementName);
        SelectionPolicy *policy = nullptr;
        if (selectionPolicy == "nve")
        {
            policy = new NaiveSelection();
        }
        else if (selectionPolicy == "bal")
        {
            policy = new BalancedSelection(0, 0, 0);
        }
        else if (selectionPolicy == "eco")
        {
            policy = new EconomySelection();
        }
        else if (selectionPolicy == "env")
        {
            policy = new SustainabilitySelection();
        }
        else
        {
            error("Cannot create this plan");
        }
        simulation.addPlan(settlement, policy);
        complete();
    }
    else
    {
        error("Cannot create this plan");
    }
}

const string AddPlan::toString() const
{
    if (getStatus() == ActionStatus::COMPLETED)
    {
        return "plan " + settlementName + " " + selectionPolicy + " COMPLETED";
    }
    else
    {
        return "plan " + settlementName + " " + selectionPolicy + " ERROR: " + getErrorMsg();
    }
}

AddPlan *AddPlan::clone() const
{
    return new AddPlan(*this);
}

AddSettlement::AddSettlement(const string &settlementName, SettlementType settlementType) : settlementName(settlementName), settlementType(settlementType) {}

void AddSettlement::act(Simulation &simulation)
{
    Settlement *settlement = new Settlement(settlementName, settlementType);
    if (!simulation.addSettlement(settlement))
    {
        delete settlement;
        error("Settlement already exists");
    }
    else
    {
        complete();
    }
}

AddSettlement *AddSettlement::clone() const
{
    return new AddSettlement(*this);
}

const std::string AddSettlement::toString() const
{
    if (getStatus() == ActionStatus::COMPLETED)
    {
        return "settlement " + settlementName + " " + std::to_string(static_cast<int>(settlementType)) + " COMPLETED";
    }
    else
    {
        return "settlement " + settlementName + " " + std::to_string(static_cast<int>(settlementType)) + " ERROR: " + getErrorMsg();
    }
}

PrintPlanStatus::PrintPlanStatus(int planId) : planId(planId) {}

void PrintPlanStatus::act(Simulation &simulation)
{
    if (!simulation.isPlanExists(planId))
    {
        error("Plan doesn’t exist");
        return;
    }
    Plan plan = simulation.getPlan(planId);
    plan.printStatus();

    complete(); // Mark action as completed
}

PrintPlanStatus *PrintPlanStatus::clone() const
{
    return new PrintPlanStatus(*this);
}

const string PrintPlanStatus::toString() const
{
    if (getStatus() == ActionStatus::COMPLETED)
    {
        return "planStatus " + std::to_string(planId) + " COMPLETED";
    }
    else
    {
        return "planStatus " + std::to_string(planId) + " ERROR: " + getErrorMsg();
    }
}

ChangePlanPolicy::ChangePlanPolicy(const int planId, const string &newPolicy) : planId(planId), newPolicy(newPolicy) {}

void ChangePlanPolicy::act(Simulation &simulation)
{
    if (!simulation.isPlanExists(planId))
    {
        error("Cannot change selection policy");
        return;
    }
    Plan plan = simulation.getPlan(planId);
    SelectionPolicy *newSelectionPolicy = nullptr;

    if (newPolicy == "nev")
    {
        newSelectionPolicy = new NaiveSelection();
    }
    else if (newPolicy == "eco")
    {
        newSelectionPolicy = new EconomySelection();
    }
    else if (newPolicy == "env")
    {
        newSelectionPolicy = new SustainabilitySelection();
    }
    else if (newPolicy == "bal")
    {
        newSelectionPolicy = new BalancedSelection(plan.getlifeQualityScore(), plan.getEconomyScore(), plan.getEnvironmentScore());
    }
    else
    {
        error("Cannot change selection policy");
        return;
    }
    if (plan.getSelectionPolicyType() == newSelectionPolicy->toString())
    {
        delete newSelectionPolicy;
        error("Cannot change selection policy");
        return;
    }
    plan.setSelectionPolicy(newSelectionPolicy);

    complete(); // Mark action as completed
}

ChangePlanPolicy *ChangePlanPolicy::clone() const
{
    return new ChangePlanPolicy(*this);
}

const std::string ChangePlanPolicy::toString() const
{
    if (getStatus() == ActionStatus::COMPLETED)
    {
        return "changePolicy " + std::to_string(planId) + " " + newPolicy + " COMPLETED";
    }
    else
    {
        return "changePolicy " + std::to_string(planId) + " " + newPolicy + " ERROR: " + getErrorMsg();
    }
}

Close::Close() {}

void Close::act(Simulation &simulation)
{
    delete backupSim;
    backupSim = nullptr;

    simulation.close();
    complete(); // Mark action as completed
}

Close *Close::clone() const
{
    return new Close(*this);
}

const string Close::toString() const
{
    return "close COMPLETED";
}

AddFacility::AddFacility(const string &facilityName, const FacilityCategory facilityCategory, const int price, const int lifeQualityScore, const int economyScore, const int environmentScore) : facilityName(facilityName), facilityCategory(facilityCategory), price(price), lifeQualityScore(lifeQualityScore), economyScore(economyScore), environmentScore(environmentScore) {}

void AddFacility::act(Simulation &simulation)
{
    FacilityType facility = FacilityType(facilityName, facilityCategory, price, lifeQualityScore, economyScore, environmentScore);
    if (!simulation.addFacility(facility))
    {
        error("Facility already exists");
    }
    else
    {
        complete();
    }
}

AddFacility *AddFacility::clone() const
{
    return new AddFacility(*this);
}

const std::string AddFacility::toString() const
{
    if (getStatus() == ActionStatus::COMPLETED)
    {
        return "facility " + facilityName + " " + std::to_string(static_cast<int>(facilityCategory)) + " " +
               std::to_string(price) + " " + std::to_string(lifeQualityScore) + " " +
               std::to_string(economyScore) + " " + std::to_string(environmentScore) + " COMPLETED";
    }
    else
    {
        return "facility " + facilityName + " " + std::to_string(static_cast<int>(facilityCategory)) + " " +
               std::to_string(price) + " " + std::to_string(lifeQualityScore) + " " +
               std::to_string(economyScore) + " " + std::to_string(environmentScore) + " ERROR: " + getErrorMsg();
    }
}

BackupSimulation::BackupSimulation() {}

void BackupSimulation::act(Simulation &simulation)
{
    if (backupSim != nullptr)
    {
        delete backupSim;
    }
    backupSim = simulation.clone();
    complete();
}

BackupSimulation *BackupSimulation::clone() const
{
    return new BackupSimulation(*this);
}

const string BackupSimulation::toString() const
{
    return "backup COMPLETED";
}

RestoreSimulation::RestoreSimulation() {}

void RestoreSimulation::act(Simulation &simulation)
{
    if (backupSim == nullptr)
    {
        error("No backup available");
        return;
    }

    simulation = *backupSim;
    complete();
}

RestoreSimulation *RestoreSimulation::clone() const
{
    return new RestoreSimulation(*this);
}

const std::string RestoreSimulation::toString() const
{
    if (getStatus() == ActionStatus::COMPLETED)
    {
        return "restore COMPLETED";
    }
    else
    {
        return "restore ERROR: " + getErrorMsg();
    }
}

PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(Simulation &simulation)
{
    const std::vector<BaseAction *> &actionsLog = simulation.getActionsLog();

    for (const BaseAction *action : actionsLog)
    {
        if (action->toString() != "log COMPLETED")
        {
            std::cout << action->toString() << std::endl;
        }
    }
    complete();
}

PrintActionsLog *PrintActionsLog::clone() const
{
    return new PrintActionsLog(*this);
}

const std::string PrintActionsLog::toString() const
{
    return "log COMPLETED";
}