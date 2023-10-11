// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
#ifndef ZOMBIE_COMPARATORS_H
#define ZOMBIE_COMPARATORS_H
#include "initialize.h"

struct PrioritizeHighNumRounds
{
    bool operator()(const Zombie* a, const Zombie* b)
    {
        if(a->numRounds != b->numRounds)
            return a->numRounds < b->numRounds;
        return a->name > b->name;
    }
};


struct PrioritizeLowNumRounds
{
    bool operator()(const Zombie* a, const Zombie* b)
    {
        if(a->numRounds != b->numRounds)
            return a->numRounds > b->numRounds; // for min
        return a->name > b->name; // for letter order
    }
};


struct PrioritizeETA {
    bool operator()(const Zombie* a, const Zombie* b) {
        uint32_t eta_a = (a->distance) / a->speed;
        uint32_t eta_b = (b->distance) / b->speed;

        if (eta_a < eta_b) {
            return false;
        } else if (eta_a > eta_b) {
            return true;
        } else { // ETA is the same, check for health
            if (a->health < b->health) {
                return false;
            } else if (a->health > b->health) {
                return true;
            } else { // Health is the same, check  name
                return a->name > b->name;
            }
        }
    }
};

std::priority_queue<Zombie*, std::vector<Zombie*>, PrioritizeETA> zombiesPriority;

#endif  // ZOMBIE_COMPARATORS_H