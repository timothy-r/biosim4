// spawnNewGeneration.cpp

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include "simulator.h"
#include "genome-neurons.h"
#include "common/include/gridBuilder.h"
#include "domain/include/genomeBuilder.h"
#include "domain/include/indivBuilder.h"
// #include "dirFactory.h"

namespace BS {

extern std::pair<bool, float> passedSurvivalCriterion(const Indiv &indiv, unsigned challenge);
// Coord findEmptyLocation(Grid grid);

// Requires that the grid, signals, and peeps containers have been allocated.
// This will erase the grid and signal layers, then create a new population in
// the peeps container at random locations with random genomes.
void initializeGeneration0()
{
    // The grid has already been allocated, just clear and reuse it
    grid.zeroFill();

    // use a GridBuilder to create the barriers
    auto barrierType = GridBuilder::barrierType(p.barrierType);

    GridBuilder gridBuilder = GridBuilder(randomUint);
    gridBuilder.createBarrier(grid, barrierType);

    // grid.createBarrier(p.barrierType);

    // The signal layers have already been allocated, so just reuse them
    signals.zeroFill();

    GenomeBuilder genomeBuilder = GenomeBuilder(
        randomUint, p.genomeInitialLengthMin, p.genomeInitialLengthMax, p.genomeMaxLength,
        p.pointMutationRate, p.geneInsertionDeletionRate, p.deletionRatio,
        p.sexualReproduction, p.chooseParentsByFitness
    );

    IndivBuilder indivBuilder = IndivBuilder(
        randomUint, grid, genomeBuilder, p.longProbeDistance
    );

    // Spawn the population. The peeps container has already been allocated,
    // just clear and reuse it
    for (uint16_t index = 1; index <= p.population; ++index) {
        
        indivBuilder.initIndividual(peeps[index], index);

        // peeps[index].initialize(
        //     index, 
        //     findEmptyLocation(grid), 
        //     genomeBuilder.makeRandomGenome(),
        //     DirFactory::random8());
    }
}

// Coord findEmptyLocation(Grid grid) {
//     Coord loc;
//     uint16_t size_x = grid.sizeX();
//     uint16_t size_y = grid.sizeY();

//     while (true) {
//         loc.x = randomUint(0, size_x - 1);
//         loc.y = randomUint(0, size_y - 1);
//         if (grid.isEmptyAt(loc)) {
//             break;
//         }
//     }
//     return loc;
// }

// Requires a container with one or more parent genomes to choose from.
// Called from spawnNewGeneration(). This requires that the grid, signals, and
// peeps containers have been allocated. This will erase the grid and signal
// layers, then create a new population in the peeps container with random
// locations and genomes derived from the container of parent genomes.
void initializeNewGeneration(const std::vector<std::shared_ptr<Genome>> &parentGenomes, unsigned generation)
{
    // extern Genome generateChildGenome(const std::vector<Genome> &parentGenomes);

    // The grid, signals, and peeps containers have already been allocated, just
    // clear them if needed and reuse the elements
    grid.zeroFill();

    auto barrierType = GridBuilder::barrierType(p.barrierType);

    GridBuilder gridBuilder = GridBuilder(randomUint);
    gridBuilder.createBarrier(grid, barrierType);

    // grid.createBarrier(p.barrierType);
    signals.zeroFill();

    GenomeBuilder genomeBuilder = GenomeBuilder(
        randomUint, p.genomeInitialLengthMin, p.genomeInitialLengthMax, p.genomeMaxLength,
        p.pointMutationRate, p.geneInsertionDeletionRate, p.deletionRatio,
        p.sexualReproduction, p.chooseParentsByFitness
    );

    IndivBuilder indivBuilder = IndivBuilder(
        randomUint, grid, genomeBuilder, p.longProbeDistance
    );

    // Spawn the population. This overwrites all the elements of peeps[]
    for (uint16_t index = 1; index <= p.population; ++index) {

        indivBuilder.reinitIndividual(peeps[index], index, parentGenomes);

        // peeps[index].initialize(
        //     index, 
        //     findEmptyLocation(grid), 
        //     genomeBuilder.generateChildGenome(parentGenomes),
        //     DirFactory::random8()
        // );
    }
}


// At this point, the deferred death queue and move queue have been processed
// and we are left with zero or more individuals who will repopulate the
// world grid.
// In order to redistribute the new population randomly, we will save all the
// surviving genomes in a container, then clear the grid of indexes and generate
// new individuals. This is inefficient when there are lots of survivors because
// we could have reused (with mutations) the survivors' genomes and neural
// nets instead of rebuilding them.
// Returns number of survivor-reproducers.
// Must be called in single-thread mode between generations.
unsigned spawnNewGeneration(unsigned generation, unsigned murderCount)
{
    unsigned sacrificedCount = 0; // for the altruism challenge

    extern void appendEpochLog(unsigned generation, unsigned numberSurvivors, unsigned murderCount);
    extern std::pair<bool, float> passedSurvivalCriterion(const Indiv &indiv, unsigned challenge);
    extern void displaySignalUse();

    // This container will hold the indexes and survival scores (0.0..1.0)
    // of all the survivors who will provide genomes for repopulation.
    std::vector<std::pair<uint16_t, float>> parents; // <indiv index, score>

    // This container will hold the genomes of the survivors
    std::vector<std::shared_ptr<Genome>> parentGenomes;

    if (p.challenge != CHALLENGE_ALTRUISM) {
        // First, make a list of all the individuals who will become parents; save
        // their scores for later sorting. Indexes start at 1.
        for (uint16_t index = 1; index <= p.population; ++index) {
            std::pair<bool, float> passed = passedSurvivalCriterion(peeps[index], p.challenge);
            // Save the parent genome if it results in valid neural connections
            // ToDo: if the parents no longer need their genome record, we could
            // possibly do a move here instead of copy, although it's doubtful that
            // the optimization would be noticeable.
            if (passed.first && !peeps[index].nnet.connections.empty()) {
                parents.push_back( { index, passed.second } );
            }
        }
    } else {
        // For the altruism challenge, test if the agent is inside either the sacrificial
        // or the spawning area. We'll count the number in the sacrificial area and
        // save the genomes of the ones in the spawning area, saving their scores
        // for later sorting. Indexes start at 1.

        bool considerKinship = true;
        std::vector<uint16_t> sacrificesIndexes; // those who gave their lives for the greater good

        for (uint16_t index = 1; index <= p.population; ++index) {
            // This the test for the spawning area:
            std::pair<bool, float> passed = passedSurvivalCriterion(peeps[index], CHALLENGE_ALTRUISM);
            if (passed.first && !peeps[index].nnet.connections.empty()) {
                parents.push_back( { index, passed.second } );
            } else {
                // This is the test for the sacrificial area:
                passed = passedSurvivalCriterion(peeps[index], CHALLENGE_ALTRUISM_SACRIFICE);
                if (passed.first && !peeps[index].nnet.connections.empty()) {
                    if (considerKinship) {
                        sacrificesIndexes.push_back(index);
                    } else {
                        ++sacrificedCount;
                    }
                }
            }
        }

        unsigned generationToApplyKinship = 10;
        constexpr unsigned altruismFactor = 10; // the saved:sacrificed ratio

        if (considerKinship) {
            if (generation > generationToApplyKinship) {
                // Todo: optimize!!!
                float threshold = 0.7;

                std::vector<std::pair<uint16_t, float>> survivingKin;
                for (unsigned passes = 0; passes < altruismFactor; ++passes) {
                    for (uint16_t sacrificedIndex : sacrificesIndexes) {
                        // randomize the next loop so we don't keep using the first one repeatedly
                        unsigned startIndex = randomUint(0, parents.size() - 1);
                        for (unsigned count = 0; count < parents.size(); ++count) {
                            const std::pair<uint16_t, float> &possibleParent = parents[(startIndex + count) % parents.size()];
                            const std::shared_ptr<Genome> g1 = peeps[sacrificedIndex].genome;
                            const std::shared_ptr<Genome> g2 = peeps[possibleParent.first].genome;
                            float similarity = genomeSimilarity(g1, g2);
                            if (similarity >= threshold) {
                                survivingKin.push_back(possibleParent);
                                // mark this one so we don't use it again?
                                break;
                            }
                        }
                    }
                }
                std::cout << parents.size() << " passed, "
                                            << sacrificesIndexes.size() << " sacrificed, "
                                            << survivingKin.size() << " saved" << std::endl; // !!!
                parents = std::move(survivingKin);
            }
        } else {
            // Limit the parent list
            unsigned numberSaved = sacrificedCount * altruismFactor;
            std::cout << parents.size() << " passed, " << sacrificedCount << " sacrificed, " << numberSaved << " saved" << std::endl; // !!!
            if (!parents.empty() && numberSaved < parents.size()) {
                parents.erase(parents.begin() + numberSaved, parents.end());
            }
        }
    }

    // Sort the indexes of the parents by their fitness scores
    std::sort(parents.begin(), parents.end(),
        [](const std::pair<uint16_t, float> &parent1, const std::pair<uint16_t, float> &parent2) {
            return parent1.second > parent2.second;
        });

    // Assemble a list of all the parent genomes. These will be ordered by their
    // scores if the parents[] container was sorted by score
    parentGenomes.reserve(parents.size());
    for (const std::pair<uint16_t, float> &parent : parents) {
        parentGenomes.push_back(peeps[parent.first].genome);
    }

    std::cout << "Gen " << generation << ", " << parentGenomes.size() << " survivors" << std::endl;
    appendEpochLog(generation, parentGenomes.size(), murderCount);
    //displaySignalUse(); // for debugging only

    // Now we have a container of zero or more parents' genomes

    if (!parentGenomes.empty()) {
        // Spawn a new generation
        initializeNewGeneration(parentGenomes, generation + 1);
    } else {
        // Special case: there are no surviving parents: start the simulation over
        // from scratch with randomly-generated genomes
        initializeGeneration0();
    }

    return parentGenomes.size();
}

} // end namespace BS
