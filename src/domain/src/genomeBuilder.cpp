#include <iostream>
#include <cassert>

#include "../include/genomeBuilder.h"

namespace BS
{
    GenomeBuilder::GenomeBuilder(RandomUintGenerator &r, unsigned min, unsigned max, unsigned gml, double pmr, double gidr, double dr, bool sr, bool cp) : 
    randomUint{r}, minLength{min}, maxLength{max}, genomeMaxLength{gml}, pointMutationRate{pmr}, geneInsertionDeletionRate{gidr}, deletionRatio{dr}, sexualReproduction{sr}, chooseParentsByFitness{cp}
    {}

    Genome GenomeBuilder::makeRandomGenome()
    {
        Genome genome;

        unsigned length = randomUint(minLength, maxLength);
        for (unsigned n = 0; n < length; ++n) {
            genome.push_back(makeRandomGene());
        }

        return genome;
    }

    Gene GenomeBuilder::makeRandomGene()
    {
        Gene gene;

        gene.sourceType = randomUint() & 1;
        gene.sourceNum = (uint16_t)randomUint(0, 0x7fff);
        gene.sinkType = randomUint() & 1;
        gene.sinkNum = (uint16_t)randomUint(0, 0x7fff);
        // gene.weight = Gene::makeRandomWeight();
        gene.weight =  randomUint(0, 0xffff) - 0x8000; 

        return gene;
    }

    // This generates a child genome from one or two parent genomes.
    // If the parameter p.sexualReproduction is true, two parents contribute
    // genes to the offspring. The new genome may undergo mutation.
    // Must be called in single-thread mode between generations
    Genome GenomeBuilder::generateChildGenome(const std::vector<Genome> &parentGenomes)
    {
        // random parent (or parents if sexual reproduction) with random
        // mutations
        Genome genome;

        uint16_t parent1Idx;
        uint16_t parent2Idx;

        // Choose two parents randomly from the candidates. If the parameter
        // p.chooseParentsByFitness is false, then we choose at random from
        // all the candidate parents with equal preference. If the parameter is
        // true, then we give preference to candidate parents according to their
        // score. Their score was computed by the survival/selection algorithm
        // in survival-criteria.cpp.
        if (chooseParentsByFitness && parentGenomes.size() > 1) {
            parent1Idx = randomUint(1, parentGenomes.size() - 1);
            parent2Idx = randomUint(0, parent1Idx - 1);
        } else {
            parent1Idx = randomUint(0, parentGenomes.size() - 1);
            parent2Idx = randomUint(0, parentGenomes.size() - 1);
        }

        const Genome &g1 = parentGenomes[parent1Idx];
        const Genome &g2 = parentGenomes[parent2Idx];

        if (g1.empty() || g2.empty()) {
            std::cout << "invalid genome" << std::endl;
            assert(false);
        }

        auto overlayWithSliceOf = [&](const Genome &gShorter) {
            uint16_t index0 = randomUint(0, gShorter.size() - 1);
            uint16_t index1 = randomUint(0, gShorter.size());
            if (index0 > index1) {
                std::swap(index0, index1);
            }
            std::copy(gShorter.begin() + index0, gShorter.begin() + index1, genome.begin() + index0);
        };

        if (sexualReproduction) {
            if (g1.size() > g2.size()) {
                genome = g1;
                overlayWithSliceOf(g2);
                assert(!genome.empty());
            } else {
                genome = g2;
                overlayWithSliceOf(g1);
                assert(!genome.empty());
            }

            // Trim to length = average length of parents
            unsigned sum = g1.size() + g2.size();
            // If average length is not an integral number, add one half the time
            if ((sum & 1) && (randomUint() & 1)) {
                ++sum;
            }
            cropLength(genome, sum / 2);
            assert(!genome.empty());
        } else {
            genome = g2;
            assert(!genome.empty());
        }

        randomInsertDeletion(genome);
        assert(!genome.empty());
        applyPointMutations(genome);
        assert(!genome.empty());
        assert(genome.size() <= genomeMaxLength);

        return genome;
    }

    // This function causes point mutations in a genome with a probability defined
    // by the parameter p.pointMutationRate.
    void GenomeBuilder::applyPointMutations(Genome &genome)
    {
        unsigned numberOfGenes = genome.size();
        while (numberOfGenes-- > 0) {
            if ((randomUint() / (float)RANDOM_UINT_MAX) < pointMutationRate) {
                randomBitFlip(genome);
            }
        }
    }

    // If the genome is longer than the prescribed length, and if it's longer
    // than one gene, then we remove genes from the front or back. This is
    // used only when the simulator is configured to allow genomes of
    // unequal lengths during a simulation.
    void GenomeBuilder::cropLength(Genome &genome, unsigned length)
    {
        if (genome.size() > length && length > 0) {
            if (randomUint() / (float)RANDOM_UINT_MAX < 0.5) {
                // trim front
                unsigned numberElementsToTrim = genome.size() - length;
                genome.erase(genome.begin(), genome.begin() + numberElementsToTrim);
            } else {
                // trim back
                genome.erase(genome.end() - (genome.size() - length), genome.end());
            }
        }
    }

    // This applies a point mutation at a random bit in a genome.
    void GenomeBuilder::randomBitFlip(Genome &genome)
    {
        int method = 1;

        unsigned byteIndex = randomUint(0, genome.size() - 1) * sizeof(Gene);
        unsigned elementIndex = randomUint(0, genome.size() - 1);
        uint8_t bitIndex8 = 1 << randomUint(0, 7);

        if (method == 0) {
            ((uint8_t *)&genome[0])[byteIndex] ^= bitIndex8;
        } else if (method == 1) {
            float chance = randomUint() / (float)RANDOM_UINT_MAX; // 0..1
            if (chance < 0.2) { // sourceType
                genome[elementIndex].sourceType ^= 1;
            } else if (chance < 0.4) { // sinkType
                genome[elementIndex].sinkType ^= 1;
            } else if (chance < 0.6) { // sourceNum
                genome[elementIndex].sourceNum ^= bitIndex8;
            } else if (chance < 0.8) { // sinkNum
                genome[elementIndex].sinkNum ^= bitIndex8;
            } else { // weight
                genome[elementIndex].weight ^= (1 << randomUint(1, 15));
            }
        } else {
            assert(false);
        }
    }

    // Inserts or removes a single gene from the genome. This is
    // used only when the simulator is configured to allow genomes of
    // unequal lengths during a simulation.
    void GenomeBuilder::randomInsertDeletion(Genome &genome)
    {
        float probability = geneInsertionDeletionRate;
        if (randomUint() / (float)RANDOM_UINT_MAX < probability) {
            if (randomUint() / (float)RANDOM_UINT_MAX < deletionRatio) {
                // deletion
                if (genome.size() > 1) {
                    genome.erase(genome.begin() + randomUint(0, genome.size() - 1));
                }
            } else if (genome.size() < genomeMaxLength) {
                // insertion
                //genome.insert(genome.begin() + randomUint(0, genome.size() - 1), makeRandomGene());
                genome.push_back(makeRandomGene());
            }
        }
    }

} // namespace BS
