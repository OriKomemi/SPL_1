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

Simulation* backup = nullptr;
// Constructor: Initialize simulation and parse the configuration file
Simulation::Simulation(const string &configFilePath) : isRunning(false), planCounter(0), actionsLog(), plans(), settlements(), facilitiesOptions(){
    ifstream configFile(configFilePath);

    if (!configFile.is_open()) {
        throw runtime_error("Failed to open configuration file: " + configFilePath);
    }

    string line;
    while (getline(configFile, line)) {
        vector<string> args = Auxiliary::parseArguments(line);

        if (args.empty()) continue;

        if (args[0] == "settlement" && args.size() == 3) {
            string name = args[1];
            int type = stoi(args[2]);
            SettlementType settlementType;

            switch (type) {
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
                    cerr << "Invalid settlement type in line: " << line << endl;
                    continue;
            }

            Settlement *settlement = new Settlement(name, settlementType);
            if (!addSettlement(settlement)) {
                delete settlement;
                cerr << "Duplicate settlement: " << name << endl;
            }
        } else if (args[0] == "facility" && args.size() == 7) {
            string name = args[1];
            int category = stoi(args[2]);
            int price = stoi(args[3]);
            int lifeQualityScore = stoi(args[4]);
            int economyScore = stoi(args[5]);
            int environmentScore = stoi(args[6]);

            FacilityCategory facilityCategory;
            switch (category) {
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
                    cerr << "Invalid facility category in line: " << line << endl;
                    continue;
            }

            FacilityType facility(name, facilityCategory, price, lifeQualityScore, economyScore, environmentScore);
            if (!addFacility(facility)) {
                cout << "Duplicate facility: " << name << endl;
            }
        } else if (args[0] == "plan" && args.size() == 3) {
            string settlementName = args[1];
            string policyType = args[2];
            if (isSettlementExists(settlementName)) {
                Settlement &settlement = getSettlement(settlementName);
                SelectionPolicy *policy = nullptr;
                if (policyType == "nve") {
                    policy = new NaiveSelection();
                } else if (policyType == "bal") {
                    policy = new BalancedSelection(0, 0, 0);
                } else if (policyType == "eco") {
                    policy = new EconomySelection();
                } else if (policyType == "env") {
                    policy = new SustainabilitySelection();
                } else {
                    cout << "Invalid selection policy in line: " << line << endl;
                    continue;
                }
                addPlan(settlement, policy);
            } else {
                cout << "Settlement: " + settlementName + " do not exists" << endl;
            }

        } else {
            cout << "Invalid line format: " << line << endl;
        }
    }

    configFile.close();
}

Simulation::Simulation(const Simulation &other)
    : isRunning(other.isRunning),
      planCounter(other.planCounter) {

    for (const auto &action : other.actionsLog) {
        actionsLog.push_back(action->clone());
    }

    for (const auto &plan : other.plans) {
        plans.push_back(plan); // Assuming Plan has a copy constructor
    }

    for (const auto &settlement : other.settlements) {
        settlements.push_back(new Settlement(*settlement));
    }

    facilitiesOptions = other.facilitiesOptions;
}


// Start the simulation
void Simulation::start() {
    isRunning = true;
    cout << "The simulation has started" << endl;

    string command;
    while (isRunning) {
        cout << "> ";
        getline(cin, command);
        vector<string> args = Auxiliary::parseArguments(command);

        if (args.empty()) continue;

        if (args[0] == "step") {
            if (args.size() != 2) {
                cout << "Invalid syntax for step command" << endl;
                continue;
            }
            int steps = stoi(args[1]);
            for(int i=0; i<steps; i++)
                step();
        } else if (args[0] == "plan") {
            // Implement plan creation command
        } else if (args[0] == "facility") {
            // Implement facility addition command
        } else if (args[0] == "settlement") {
            // Implement settlement addition command
        } else if (args[0] == "close") {
            close();
        } else {
            cout << "Unknown command: " << args[0] << endl;
        }
    }
}

// Add a plan to the simulation
void Simulation::addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy) {
    Plan plan(planCounter++, settlement, selectionPolicy, facilitiesOptions);
    plans.push_back(plan);
}

// Add an action (not fully implemented)
void Simulation::addAction(BaseAction *action) {
    actionsLog.push_back(action);
}

// Add a settlement to the simulation
bool Simulation::addSettlement(Settlement *settlement) {
    if (isSettlementExists(settlement->getName())) {
        return false;
    }
    settlements.push_back(settlement);
    return true;
}

// Add a facility type to the simulation
bool Simulation::addFacility(FacilityType facility) {
    for (const auto &existingFacility : facilitiesOptions) {
        if (existingFacility.getName() == facility.getName()) {
            return false; // Duplicate facility
        }
    }
    facilitiesOptions.push_back(facility);
    return true;
}

// Check if a settlement exists
bool Simulation::isSettlementExists(const string &settlementName) {
    for (const auto &settlement : settlements) {
        if (settlement->getName() == settlementName) {
            return true;
        }
    }
    return false;
}

// Retrieve a settlement by name
Settlement &Simulation::getSettlement(const string &settlementName) {
    for (auto &settlement : settlements) {
        if (settlement->getName() == settlementName) {
            return *settlement;
        }
    }
    cout << ("Settlement not found: " + settlementName) << endl;
}

bool Simulation::isPlanExisit(const int planID)
{
    for (auto &plan : plans) {
        if (plan.getPlanId() == planID) {
            return true;
        }
    }
    return false;
}

// Retrieve a plan by ID
Plan &Simulation::getPlan(const int planID) {
    for (auto &plan : plans) {
        if (plan.getPlanId() == planID) {
            return plan;
        }
    }
    cout << "Plan not found: " + to_string(planID) << endl;
}

// Step through the simulation
void Simulation::step() {
    for (auto &plan : plans) {
        plan.step();
    }
}

// Close the simulation
void Simulation::close() {
    isRunning = false;
    cout << "Simulation closed" << endl;
}

// Open the simulation (not fully implemented)
void Simulation::open() {
    cerr << "open not implemented yet" << endl;
}

Simulation *Simulation::clone() const
{
    return new Simulation(*this);
}
