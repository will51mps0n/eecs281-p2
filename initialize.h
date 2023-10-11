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

std::vector<Zombie> initializeZombies(uint32_t &quiverCapacity, int &startingRound)
{
    std::string comment, label, delimiter;

    // For random zombie generator
    uint32_t randomSeed, maxRandDistance, maxRandSpeed, maxRandHealth;

    std::getline(std::cin, comment);

    std::cin >> label >> quiverCapacity;

    std::cin >> label >> randomSeed;

    std::cin >> label >> maxRandDistance;

    bool isGameLost = false; // Initialize the game lost flag here

    // Check if maxRandDistance is set to 0.
    if (maxRandDistance == 0) {
        isGameLost = true; // Set game lost flag if maxRandDistance is zero
    }

    std::cin >> label >> maxRandSpeed;

    std::cin >> label >> maxRandHealth;

    P2random::initialize(randomSeed, maxRandDistance, maxRandSpeed, maxRandHealth);

    std::vector<Zombie> zombies;
    int roundNumber;

    bool firstRoundEncountered = false;  // Flag to check if the first round is encountered

    while (std::cin >> delimiter)
    {
        if (delimiter != "---")
        {
            std::cerr << "Error: Unexpected format in input file" << std::endl;
            break;
        }

        std::cin >> label >> roundNumber;
        if (!firstRoundEncountered) {
            startingRound = roundNumber;  // Set starting round based on the first round encountered
            firstRoundEncountered = true;  // Set the flag to true since the first round is now encountered
        }

        uint32_t numRandomZombies, numNamedZombies;
        std::cin >> label >> numRandomZombies;
        std::cin >> label >> numNamedZombies;

        // Check if there are random zombies to be created with maxRandDistance as 0
        if (isGameLost && numRandomZombies > 0) {
            std::cerr << "Error: Game Lost! Random zombies cannot be created when maxRandDistance is set to 0" << std::endl;
            exit(1);  // Exit the program with an error code or provide any other mechanism to signify game loss
        }

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
    }

    return zombies;
}
