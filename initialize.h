// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#pragma once
#include "P2random.h"
#include <vector>
#include <string>

struct Args
{
    bool helpMode = false;
    bool verboseMode = false;
    bool statistics = false;
    size_t statisticsNum = 0;
    bool median = false;
};

struct Zombie
{
    std::string name;
    uint32_t distance;
    uint32_t speed;
    uint32_t health;
    int roundCreated;
    int numRounds = 1;

    Zombie(std::string n, uint32_t d, uint32_t s, uint32_t h, int rC)
        : name(n), distance(d), speed(s), health(h), roundCreated(rC), numRounds(1) {}
};
class initialize
{
private:
    /* data */
    uint32_t randomSeed, maxRandDistance, maxRandSpeed, maxRandHealth;
    std::string comment, label, delimiter;
    int numReadIn = 1;
    int roundToEncounter;

public:
    initialize();
    ~initialize();
    void firstInitialize(uint32_t &quiverCapacity, int &startingRound);
    bool initializeZombiesRound(int roundNumber, std::vector<Zombie> &zombies);
};

initialize::initialize()
{
    roundToEncounter = -1;
}

initialize::~initialize()
{
}

void initialize::firstInitialize(uint32_t &quiverCapacity, int &startingRound)
{
    //get comment
    std::getline(std::cin, comment);

    std::cin >> label >> quiverCapacity;

    std::cin >> label >> randomSeed;

    std::cin >> label >> maxRandDistance;

    std::cin >> label >> maxRandSpeed;

    std::cin >> label >> maxRandHealth;

    P2random::initialize(randomSeed, maxRandDistance, maxRandSpeed, maxRandHealth);

    std::vector<Zombie> zombies;

    std::cin >> delimiter;
    std::cin >> label >> startingRound;
    roundToEncounter = startingRound;
}

bool initialize::initializeZombiesRound(int roundNumber, std::vector<Zombie> &zombies)
{
    // Initialize the game lost flag here
    // first call to function have num read in 0
    // round num first round ==0 as well.    std::getline(std::cin, comment);

    if (roundNumber != roundToEncounter)
    {
        return false;
    }

    uint32_t numRandomZombies, numNamedZombies;
    std::cin >> label >> numRandomZombies;
    std::cin >> label >> numNamedZombies;

    // Adding random zombies to vector
    for (uint32_t i = 0; i < numRandomZombies; i++)
    {
        std::string randomName = P2random::getNextZombieName();
        uint32_t distance = P2random::getNextZombieDistance();
        uint32_t speed = P2random::getNextZombieSpeed();
        uint32_t health = P2random::getNextZombieHealth();

        Zombie newRandom(randomName, distance, speed, health, roundNumber);

        zombies.push_back(newRandom);
    }

    // Add named zombies
    for (uint32_t i = 0; i < numNamedZombies; ++i)
    {
        std::string name;
        uint32_t distance, speed, health;
        std::cin >> name >> label >> distance >> label >> speed >> label >> health;
        Zombie newNamed(name, distance, speed, health, roundNumber);
        zombies.push_back(newNamed);
    }
    std::cin >> delimiter;
    if (std::cin.eof()) {
        return true;
    }
    std::cin >> label >> roundToEncounter;
    return false;
}
