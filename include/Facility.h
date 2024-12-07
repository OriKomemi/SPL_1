#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;

enum class FacilityStatus {
    UNDER_CONSTRUCTIONS,
    OPERATIONAL,
};

enum class FacilityCategory {
    LIFE_QUALITY,
    ECONOMY,
    ENVIRONMENT,
};


class FacilityType {
    public:
        FacilityType(const string &name, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score);
        // FacilityType(const FacilityType& other);
        // FacilityType(const FacilityType &&other) noexcept;

        // FacilityType &operator=(FacilityType &&other) noexcept;
        // FacilityType &operator=(const FacilityType &other);

        const string &getName() const;
        int getCost() const;
        int getlifeQualityScore() const;
        int getEnvironmentScore() const;
        int getEconomyScore() const;
        FacilityCategory getCategory() const;

    protected:
        const string name;
        const FacilityCategory category;
        const int price;
        const int lifeQuality_score;
        const int economy_score;
        const int environment_score;
};



class Facility: public FacilityType {

    public:
        Facility(const string &name, const string &settlementName, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score);
        Facility(const FacilityType &type, const string &settlementName);
        const string &getSettlementName() const;
        const int getTimeLeft() const;
        FacilityStatus step();
        Facility* clone() const;
        void setStatus(FacilityStatus status);
        const FacilityStatus& getStatus() const;
        const string toString() const;

    private:
        const string settlementName;
        FacilityStatus status;
        int timeLeft;
};