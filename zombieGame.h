// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#include <queue>
#include <iostream>
#include <deque>
#include "initialize.h"
#include "zombieComparators.h"

class ZombieGame
{
private:
    std::priority_queue<int> lowerHalf;                                      // max
    std::priority_queue<int, std::vector<int>, std::greater<int>> upperHalf; // min
    std::priority_queue<Zombie *, std::vector<Zombie *>, PrioritizeHighNumRounds> zombiesHighRounds;
    std::priority_queue<Zombie *, std::vector<Zombie *>, PrioritizeLowNumRounds> zombiesLowRounds;
    std::priority_queue<Zombie *, std::vector<Zombie *>, PrioritizeETA> zombiesPriority;
    std::vector<Zombie> firstKilled;
    std::deque<Zombie> lastKilled;
    Args arguments;
    std::vector<Zombie> &zombies;
    uint32_t quiverCapacity;
    int starting_round;
    int roundNum;
    Zombie *zombieLoser;
    Zombie *zombieWinner;
    bool brainsEaten;
    size_t zombieIndex;
    uint32_t currentQuiver;
    size_t zombiesActive = 0;

    void advance_zombies();
    void new_zombies();
    void print_killed_statistics();
    void print_active_statistics();
    void rebuild_priority_queues();
    void shoot_zombies();
    void print_loss();
    double get_median();

public:
    ZombieGame(Args args, std::vector<Zombie> &zmb, uint32_t qCapacity, int startingRound)
        : arguments(args), zombies(zmb),
          quiverCapacity(qCapacity), starting_round(startingRound), roundNum(startingRound),
          zombieLoser(nullptr), brainsEaten(false),
          zombieIndex(0), currentQuiver(0) {}

    void play_game();
};

void ZombieGame::play_game()
{
    if (starting_round != 1 && arguments.verboseMode)
    {
        for (int i = 1; i < roundNum; i++)
            std::cout << "Round: " << i << std::endl;
    }
    bool gameWon = false;

    while (!gameWon)
    {
        if (arguments.verboseMode)
        {
            std::cout << "Round: " << roundNum << std::endl;
        }
        currentQuiver = quiverCapacity;
        if (roundNum > 1)
        {
            advance_zombies();
        }

        if (brainsEaten)
        {
            zombiesActive = zombiesPriority.size();
            print_loss();
            return;
        }

        new_zombies();

        if (brainsEaten)
        {
            zombiesActive = zombiesPriority.size();
            print_loss();
            return;
        }

        // shoot zombies
        shoot_zombies();

        if (arguments.median && (lowerHalf.size() || upperHalf.size()))
        {
            std::cout << "At the end of round " << roundNum << ", the median zombie lifetime is " << get_median() << std::endl;
        }
        if (zombiesPriority.empty() && roundNum > zombies.back().roundCreated - 1) // changed to -1 to fix test case
        {
            gameWon = true;
            std::cout << "VICTORY IN ROUND " << roundNum
                      << "! " << zombieLoser->name
                      << " was the last zombie." << std::endl;
            if (arguments.statistics)
            {
                print_killed_statistics();
                print_active_statistics();
            }
        }
        roundNum++;
    }
}

// this should fix memory overtime with shooting zombies:: ie - MS8
void ZombieGame::shoot_zombies()
{
    while (currentQuiver != 0)
    {
        while (!zombiesPriority.empty() && zombiesPriority.top()->health < 1)
        {
            zombiesPriority.pop();
        }

        // Check if the priority queue is empty before getting the top element
        if (zombiesPriority.empty())
        {
            break; // Exit the loop if there are no more zombies to target
        }
        uint32_t current_volley;
        Zombie *zombieTarget = zombiesPriority.top();

        // two options for shooting zombies volley -> capacity is greater or less than health
        if (currentQuiver >= zombieTarget->health)
        {
            current_volley = zombieTarget->health;
        }
        else
        {
            current_volley = currentQuiver;
        }
        currentQuiver -= current_volley;
        zombieTarget->health -= current_volley;

        if (zombieTarget->health == 0)
        {
            if (arguments.median)
            {
                int zombieLifetime = zombieTarget->numRounds;
                if (lowerHalf.empty() || zombieLifetime <= lowerHalf.top())
                {
                    lowerHalf.push(zombieLifetime);
                }
                else
                {
                    upperHalf.push(zombieLifetime);
                }

                while (lowerHalf.size() > upperHalf.size() + 1)
                {
                    upperHalf.push(lowerHalf.top());
                    lowerHalf.pop();
                }

                while (upperHalf.size() > lowerHalf.size())
                {
                    lowerHalf.push(upperHalf.top());
                    upperHalf.pop();
                }
            }

            if (arguments.statistics)
            {
                if (firstKilled.size() < arguments.statisticsNum)
                {
                    firstKilled.push_back(*zombieTarget);
                }

                lastKilled.push_back(*zombieTarget);
                if (lastKilled.size() > arguments.statisticsNum)
                {
                    lastKilled.pop_front();
                }


                zombiesLowRounds.push(zombieTarget);
                zombiesHighRounds.push(zombieTarget);
            }

            if (arguments.verboseMode)
            {
                std::cout << "Destroyed: " << zombieTarget->name << " (distance: "
                          << zombieTarget->distance << ", speed: " << zombieTarget->speed
                          << ", health: " << zombieTarget->health << ")" << std::endl;
            }

            // Check if the priority queue is empty before getting the top element and popping
            if (!zombiesPriority.empty())
            {
                zombieLoser = zombiesPriority.top();
                zombiesPriority.pop();
            }
            else
            {
                zombieLoser = nullptr;
            }
        }
    }
}

void ZombieGame::advance_zombies()
{
    size_t advanceIndex = 0;
    while (advanceIndex < zombies.size() && zombies[advanceIndex].roundCreated < roundNum)
    {
        Zombie &z = zombies[advanceIndex];
        if (z.health > 0)
        {
            if (z.distance > 0)
            {
                z.distance = z.distance - std::min(z.distance, z.speed);
            }
            ++z.numRounds;
            if (arguments.verboseMode)
            {
                std::cout << "Moved: " << z.name << " (distance: "
                          << z.distance << ", speed: " << z.speed << ", health: "
                          << z.health << ")" << std::endl;
            }
            if (z.distance == 0 && !brainsEaten)
            {
                brainsEaten = true;
                zombieWinner = &zombies[advanceIndex];
            }
        }
        advanceIndex++;
    }
    return;
}

void ZombieGame::new_zombies()
{
    while (zombieIndex < zombies.size() && zombies[zombieIndex].roundCreated <= roundNum) // change from <=
    {
        // zombiesHighRounds.push(&zombies[zombieIndex]);
        // zombiesLowRounds.push(&zombies[zombieIndex]);
        //  Debugging: std::cout << "Index: " << zombieIndex << ", Zombie Name: " << zombies[zombieIndex].name << ", Round Created: " << zombies[zombieIndex].roundCreated << std::endl; // Debugging line
        if (zombies[zombieIndex].distance == 0)
        {
            brainsEaten = true;
            zombieWinner = &zombies[zombieIndex];
        }
        zombiesPriority.push(&zombies[zombieIndex]);
        zombies[zombieIndex].roundCreated = roundNum;
        if (arguments.verboseMode)
        {
            std::cout << "Created: " << zombies[zombieIndex].name << " (distance: "
                      << zombies[zombieIndex].distance << ", speed: " << zombies[zombieIndex].speed
                      << ", health: " << zombies[zombieIndex].health << ")" << std::endl;
        }

        zombieIndex++;
    }
}

void ZombieGame::print_killed_statistics()
{
    size_t numZombies = arguments.statisticsNum;
    std::cout << "Zombies still active: " << zombiesActive << std::endl;
    std::cout << "First zombies killed:" << std::endl;

    if (numZombies > firstKilled.size())
    {
        for (size_t i = 0; i < firstKilled.size(); i++)
        {
            std::cout << firstKilled[i].name
                      << " " << i + 1
                      << std::endl;
        }
    }
    else if (numZombies <= firstKilled.size())
    {
        for (size_t i = 0; i < numZombies; i++)
        {
            std::cout << firstKilled[i].name
                      << " " << i + 1
                      << std::endl;
        }
    }

    // Last zombies killed

    std::cout << "Last zombies killed:" << std::endl;

    if (numZombies >= lastKilled.size())
    {
        size_t zombiePlace = lastKilled.size();
        while (!lastKilled.empty())
        {
            std::cout << lastKilled.back().name
                      << " " << zombiePlace--
                      << std::endl;
            lastKilled.pop_back();
        }
    }
    else if (numZombies < lastKilled.size())
    {
        size_t zombiePlace = numZombies;
        for (size_t i = 0; i < numZombies; i++)
        {
            std::cout << lastKilled[lastKilled.size() - 1 - i].name
                      << " " << zombiePlace
                      << std::endl;
            --zombiePlace;
        }
    }
    return;
}

// had a bug dealing with pq not dyanmically changing order as rounds change
// this funct. isnt good for runtime, change later:
/*void ZombieGame::rebuild_priority_queues()
{

    while (!zombiesHighRounds.empty())
        zombiesHighRounds.pop();
    while (!zombiesLowRounds.empty())
        zombiesLowRounds.pop();

    for (auto &zombie : zombies)
    {
        zombiesHighRounds.push(&zombie);
        zombiesLowRounds.push(&zombie);
    }
}*/

void ZombieGame::print_active_statistics()
{
    //rebuild_priority_queues();
    size_t numZombies = arguments.statisticsNum;

    // print zombies high rounds
    std::cout << "Most active zombies:" << std::endl;
    for (size_t i = 0; i < numZombies; i++)
    {
        if (zombiesHighRounds.empty())
            break;

        std::cout << zombiesHighRounds.top()->name
                  << " " << zombiesHighRounds.top()->numRounds
                  << std::endl;
        zombiesHighRounds.pop();
    }

    // low rounds stats:
    std::cout << "Least active zombies:" << std::endl;

    for (size_t i = 0; i < numZombies; i++)
    {
        if (zombiesLowRounds.empty())
            break;

        std::cout << zombiesLowRounds.top()->name
                  << " " << zombiesLowRounds.top()->numRounds
                  << std::endl;
        zombiesLowRounds.pop();
    }
}

double ZombieGame::get_median()
{
    if (lowerHalf.size() > upperHalf.size())
    {
        return static_cast<double>(lowerHalf.top());
    }
    else if (upperHalf.size() > lowerHalf.size())
    {
        return static_cast<double>(upperHalf.top());
    }
    else if (!lowerHalf.empty() && !upperHalf.empty())
    {
        return (static_cast<double>(lowerHalf.top()) + static_cast<double>(upperHalf.top())) / 2.0;
    }
    return 0.0; // both emptty
}

void ZombieGame::print_loss()
{
    std::cout << "DEFEAT IN ROUND " << roundNum
              << "! " << zombieWinner->name
              << " ate your brains!" << std::endl;
    /*
    if we lose, there are zobues we didnt kill and therefore 
    must push into active vectors
    */
    if (arguments.statistics)
    {
        while (!zombiesPriority.empty()) {
            //named zombert, because its a zombie whos name used to be bert,
            //just a play on the word zombie... haha
            Zombie* zombert = zombiesPriority.top();
            zombiesHighRounds.push(zombert);
            zombiesLowRounds.push(zombert);
            zombiesPriority.pop();
        }
        print_killed_statistics();
        print_active_statistics();
    }
    return;
}

