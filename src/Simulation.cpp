#include "Simulation.h"
#include "Auxiliary.h"
#include "Plan.h"
#include "SelectionPolicy.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <limits> // For numeric_limits

using namespace std;

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
                cerr << "Duplicate facility: " << name << endl;
            }
        } else if (args[0] == "plan" && args.size() == 3) {
            string settlementName = args[1];
            string policyType = args[2];

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
                cerr << "Invalid selection policy in line: " << line << endl;
                continue;
            }

            addPlan(settlement, policy);
        } else {
            cerr << "Invalid line format: " << line << endl;
        }
    }

    configFile.close();
}

// Start the simulation
void Simulation::start() {
    isRunning = true;
    cout << "The simulation has started" << endl;
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
    throw runtime_error("Settlement not found: " + settlementName);
}

// Retrieve a plan by ID
Plan &Simulation::getPlan(const int planID) {
    for (auto &plan : plans) {
        if (plan.getPlanId() == planID) {
            return plan;
        }
    }
    throw runtime_error("Plan not found: " + to_string(planID));
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
