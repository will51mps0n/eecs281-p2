// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
#include <iostream>
#include <getopt.h>
#include "initialize.h"
#include "zombieGame.h"
std::vector<Zombie> initializeZombies()
{
    // read in # header
    std::string comment;
    std::getline(std::cin, comment);
    uint32_t quiverCapacity;
    uint32_t randomSeed, maxRandDistance, maxRandSpeed, maxRandHealth;

    std::cin >> quiverCapacity >> randomSeed >> maxRandDistance >> maxRandSpeed >> maxRandHealth;

    P2random::initialize(randomSeed, maxRandDistance, maxRandSpeed, maxRandHealth);

    std::vector<Zombie> zombies;

    std::string delimiter;
    int roundNumber;
    while (std::cin >> delimiter >> roundNumber)
    {
        uint32_t numRandomZombies, numNamedZombies;
        std::cin >> numRandomZombies >> numNamedZombies;

        // adding random zombies to vector
        for (uint32_t i = 0; i < numRandomZombies; i++)
        {
            std::string randomName = P2random::getNextZombieName();
            uint32_t distance = P2random::getNextZombieDistance();
            uint32_t speed = P2random::getNextZombieSpeed();
            uint32_t health = P2random::getNextZombieHealth();

            Zombie newRandom(randomName, distance, speed, health, roundNumber);

            // add new random zombie to our vector
            zombies.push_back(newRandom);
        }
        for (uint32_t i = 0; i < numNamedZombies; ++i)
        {
            std::string name;
            uint32_t distance, speed, health;
            std::cin >> name >> distance >> speed >> health;
            Zombie newNamed(name, distance, speed, health, roundNumber);
            zombies.push_back(newNamed);
        }
    }
    return zombies;
}


void print_help() {
    std::cout << "Usage: ./zombbb [OPTIONS] < INPUT" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help            Show this help message" << std::endl;
    std::cout << "  -v, --verbose         Print out extra messages during the operation" << std::endl;
    std::cout << "  -s <num>, --statistics <num> Print out statistics at the end of the program with <num> entries for each statistic" << std::endl;
    std::cout << "  -m, --median          Print out messages indicating the median time zombies have been active" << std::endl;
}

Args parse_args(int argc, char **argv) {
    int c;
    Args arguments;

    static struct option long_options[] = {
        {"help",   no_argument,       0, 'h'},
        {"verbose",  no_argument,       0, 'v'},
        {"median",  no_argument,       0, 'm'},
        {"statistics", required_argument, 0, 's'},
        {0,        0,                 0,  0 }
    };

    while (true) {
        int option_index = 0;
        c = getopt_long(argc, argv, "hmvs:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 'h':
                print_help();
                arguments.helpMode = true;
                break;

            case 'm':
                arguments.median = true;
                break;

            case 'v':
                arguments.verboseMode = true;
                break;

            case 's':

                arguments.statistics = true;
                //  Need to Convert argument to unsigned int  
                arguments.statisticsNum = std::stoul(optarg);  
                if (arguments.statisticsNum == 0) {
                    std::cerr << "Error: Invalid number for statistics" << std::endl;
                    exit(1);
                }
                break;

            default:
                std::cerr << "Error: Unknown option" << std::endl;
                exit(1);
        }
    }
    return arguments;
}


int main(int argc, char **argv) {
    Args arguments = parse_args(argc, argv);
    uint32_t quiverCapacity = 0;
    int startingRound = 1; // Default starting round
    //std::cout << "hello world" << std::endl;
    std::vector<Zombie> zombieVector = initializeZombies(quiverCapacity, startingRound);
    ZombieGame zombieGame(arguments, zombieVector, quiverCapacity, startingRound);
    zombieGame.play_game();
    return 0;
}

