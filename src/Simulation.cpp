#include "Simulation.h"
#include "Auxiliary.h"
#include "Plan.h"
#include "SelectionPolicy.h"
#include "Action.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <limits> // For numeric_limits

using namespace std;

Simulation *backupSim = nullptr;
// Constructor: Initialize simulation and parse the configuration file
Simulation::Simulation(const string &configFilePath) : isRunning(false), planCounter(0), actionsLog(), plans(), settlements(), facilitiesOptions()
{
    ifstream configFile(configFilePath);

    if (!configFile.is_open())
    {
        throw runtime_error("Failed to open configuration file: " + configFilePath);
    }

    string line;
    while (getline(configFile, line))
    {
        vector<string> args = Auxiliary::parseArguments(line);

        if (args.empty())
            continue;

        if (args[0] == "settlement" && args.size() == 3)
        {
            string name = args[1];
            int type = stoi(args[2]);
            SettlementType settlementType;

            switch (type)
            {
            case 0:
                settlementType = SettlementType::VILLAGE;
                break;
            case 1:
                settlementType = SettlementType::CITY;
                break;
            case 2:
                settlementType = SettlementType::METROPOLIS;
                break;
            default:
                cout << "Invalid settlement type in line: " << line << endl;
                continue;
            }

            Settlement *settlement = new Settlement(name, settlementType);
            if (!addSettlement(settlement))
            {
                delete settlement;
                cout << "Duplicate settlement: " << name << endl;
            }
        }
        else if (args[0] == "facility" && args.size() == 7)
        {
            string name = args[1];
            int category = stoi(args[2]);
            int price = stoi(args[3]);
            int lifeQualityScore = stoi(args[4]);
            int economyScore = stoi(args[5]);
            int environmentScore = stoi(args[6]);

            FacilityCategory facilityCategory;
            switch (category)
            {
            case 0:
                facilityCategory = FacilityCategory::LIFE_QUALITY;
                break;
            case 1:
                facilityCategory = FacilityCategory::ECONOMY;
                break;
            case 2:
                facilityCategory = FacilityCategory::ENVIRONMENT;
                break;
            default:
                cout << "Invalid facility category in line: " << line << endl;
                continue;
            }

            FacilityType facility(name, facilityCategory, price, lifeQualityScore, economyScore, environmentScore);
            if (!addFacility(facility))
            {
                cout << "Duplicate facility: " << name << endl;
            }
        }
        else if (args[0] == "plan" && args.size() == 3)
        {
            string settlementName = args[1];
            string policyType = args[2];
            if (isSettlementExists(settlementName))
            {
                Settlement &settlement = getSettlement(settlementName);
                SelectionPolicy *policy = nullptr;
                if (policyType == "nve")
                {
                    policy = new NaiveSelection();
                }
                else if (policyType == "bal")
                {
                    policy = new BalancedSelection(0, 0, 0);
                }
                else if (policyType == "eco")
                {
                    policy = new EconomySelection();
                }
                else if (policyType == "env")
                {
                    policy = new SustainabilitySelection();
                }
                else
                {
                    cout << "Invalid selection policy in line: " << line << endl;
                    continue;
                }
                addPlan(settlement, policy);
            }
            else
            {
                cout << "Settlement: " + settlementName + " do not exists" << endl;
            }
        }
        else
        {
            cout << "Invalid line format: " << line << endl;
        }
    }

    configFile.close();
}



Simulation::Simulation(const Simulation &other)
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(),
      plans(),
      settlements(),
      facilitiesOptions(other.facilitiesOptions)
{
    // Deep copy actionsLog
    actionsLog.reserve(other.actionsLog.size());
    for (auto *action : other.actionsLog) {
        actionsLog.push_back(action->clone());
    }

    // Deep copy settlements
    settlements.reserve(other.settlements.size());
    for (Settlement *sett : other.settlements) {
        settlements.push_back(new Settlement(*sett));
    }

    // Now that we have our copied settlements and have facilitiesOptions copied,
    // we can deep copy each plan using Plan::cloneDeep().
    plans.reserve(other.plans.size());
    for (const auto &plan : other.plans) {
        // cloneDeep will find the corresponding settlement inside 'settlements'
        // and create a fully deep-copied Plan.
        Plan newPlan = plan.cloneDeep(settlements, facilitiesOptions);
        plans.push_back(std::move(newPlan));
    }
}

Simulation &Simulation::operator=(const Simulation &other)
{
    if (this == &other) {
        return *this; // Self-assignment check
    }

    // Cleanup current resources
    for (auto *action : actionsLog) {
        delete action;
    }
    actionsLog.clear();

    for (auto *settlement : settlements) {
        delete settlement;
    }
    settlements.clear();

    plans.clear();
    facilitiesOptions.clear();

    // Copy simple fields
    isRunning = other.isRunning;
    planCounter = other.planCounter;

    for (const FacilityType &facility : other.facilitiesOptions)
    {
        facilitiesOptions.push_back(facility); // Uses copy constructor
    }

    // Deep copy actionsLog
    actionsLog.reserve(other.actionsLog.size());
    for (auto *action : other.actionsLog) {
        actionsLog.push_back(action->clone());
    }

    // Deep copy settlements
    settlements.reserve(other.settlements.size());
    for (Settlement *sett : other.settlements) {
        settlements.push_back(new Settlement(*sett));
    }

    // Deep copy plans using Plan::cloneDeep()
    plans.reserve(other.plans.size());
    for (const auto &plan : other.plans) {
        // Use cloneDeep for a fully deep-copied Plan
        Plan newPlan = plan.cloneDeep(settlements, facilitiesOptions);
        plans.push_back(std::move(newPlan));
    }

    return *this;
}

Simulation::Simulation(Simulation &&other) noexcept
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(std::move(other.actionsLog)),
      plans(std::move(other.plans)),
      settlements(std::move(other.settlements)),
      facilitiesOptions(std::move(other.facilitiesOptions))
{
    other.isRunning = false;
    other.planCounter = 0;
}

Simulation &Simulation::operator=(Simulation &&other) noexcept
{
    if (this != &other)
    {
        isRunning = other.isRunning;
        planCounter = other.planCounter;
        actionsLog = std::move(other.actionsLog);
        plans = std::move(other.plans);
        settlements = std::move(other.settlements);
        facilitiesOptions = std::move(other.facilitiesOptions);

        other.isRunning = false;
        other.planCounter = 0;
    }
    return *this;
}

Simulation::~Simulation()
{
    clear();
}

void Simulation::clear()
{
    for (BaseAction *action : actionsLog)
    {
        delete action;
    }
    actionsLog.clear();

    for (Settlement *settlement : settlements)
    {
        delete settlement;
    }
    settlements.clear();
    plans.clear();
    facilitiesOptions.clear();
}

// Start the simulation
void Simulation::start()
{
    isRunning = true;
    std::string command;

    std::cout << "The simulation has started" << std::endl;

    while (isRunning)
    {
        std::cout << "> ";
        std::getline(std::cin, command);

        vector<string> args = Auxiliary::parseArguments(command);

        if (args.empty())
            continue;

        // Match commands and execute actions
        BaseAction *action = nullptr;

        if (args[0] == "step" && args.size() == 2)
        {
            int steps = std::stoi(args[1]);
            action = new SimulateStep(steps);
        }
        else if (args[0] == "plan" && args.size() == 3)
        {
            action = new AddPlan(args[1], args[2]);
        }
        else if (args[0] == "settlement" && args.size() == 3)
        {
            SettlementType type = static_cast<SettlementType>(std::stoi(args[2]));
            action = new AddSettlement(args[1], type);
        }
        else if (args[0] == "facility" && args.size() == 7)
        {
            FacilityCategory category = static_cast<FacilityCategory>(std::stoi(args[2]));
            int price = std::stoi(args[3]);
            int lifeQualityScore = std::stoi(args[4]);
            int economyScore = std::stoi(args[5]);
            int environmentScore = std::stoi(args[6]);
            action = new AddFacility(args[1], category, price, lifeQualityScore, economyScore, environmentScore);
        }
        else if (args[0] == "planStatus" && args.size() == 2)
        {
            int planID = std::stoi(args[1]);
            action = new PrintPlanStatus(planID);
        }
        else if (args[0] == "changePolicy" && args.size() == 3)
        {
            int planID = std::stoi(args[1]);
            action = new ChangePlanPolicy(planID, args[2]);
        }
        else if (args[0] == "log" && args.size() == 1)
        {
            action = new PrintActionsLog();
        }
        else if (args[0] == "backup" && args.size() == 1)
        {
            action = new BackupSimulation();
        }
        else if (args[0] == "restore" && args.size() == 1)
        {
            action = new RestoreSimulation();
        }
        else if (args[0] == "close" && args.size() == 1)
        {
            action = new Close();
        }
        else
        {
            std::cout << "Unknown command: " << args[0] << std::endl;
            continue;
        }

        // Execute the action and add to the log
        if (action)
        {
            action->act(*this);
            actionsLog.push_back(action);
            // std::cout << action->toString() << std::endl;
        }
    }
}

// Add a plan to the simulation
void Simulation::addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy)
{
    Plan plan(planCounter++, settlement, selectionPolicy, facilitiesOptions);
    plans.push_back(plan);
}

// Add an action (not fully implemented)
void Simulation::addAction(BaseAction *action)
{
    actionsLog.push_back(action);
}

// Add a settlement to the simulation
bool Simulation::addSettlement(Settlement *settlement)
{
    if (isSettlementExists(settlement->getName()))
    {
        return false;
    }
    settlements.push_back(settlement);
    return true;
}

// Add a facility type to the simulation
bool Simulation::addFacility(FacilityType facility)
{
    for (const auto &existingFacility : facilitiesOptions)
    {
        if (existingFacility.getName() == facility.getName())
        {
            return false; // Duplicate facility
        }
    }
    facilitiesOptions.push_back(facility);
    return true;
}

// Check if a settlement exists
bool Simulation::isSettlementExists(const string &settlementName)
{
    for (const auto &settlement : settlements)
    {
        if (settlement->getName() == settlementName)
        {
            return true;
        }
    }
    return false;
}

bool Simulation::isPlanExists(const int planID)
{
    if (planID > planCounter - 1)
    {
        return false;
    }
    return true;
}

// Retrieve a settlement by name
Settlement &Simulation::getSettlement(const string &settlementName)
{
    for (auto &settlement : settlements)
    {
        if (settlement->getName() == settlementName)
        {
            return *settlement;
        }
    }
    throw std::runtime_error("Settlement not found");
}

// Retrieve a plan by ID
Plan &Simulation::getPlan(const int planID)
{
    for (auto &plan : plans)
    {
        if (plan.getPlanId() == planID)
        {
            return plan;
        }
    }
    throw std::runtime_error("Plan not found: " + to_string(planID));
}

const std::vector<BaseAction *> &Simulation::getActionsLog() const
{
    return actionsLog;
}

// Step through the simulation
void Simulation::step()
{
    for (auto &plan : plans)
    {
        plan.step();
    }
}

// Close the simulation
void Simulation::close()
{
    for (auto &plan : plans)
    {
        plan.printShortStatus();
    }
    isRunning = false;
}

// Open the simulation (not fully implemented)
void Simulation::open()
{
    cout << "open not implemented yet" << endl;
}

Simulation *Simulation::clone() const
{
    return new Simulation(*this);
}
