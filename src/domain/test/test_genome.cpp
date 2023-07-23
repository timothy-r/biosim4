#include "../include/genome.h"
#include "./test_helper.h"

#include <catch2/catch_test_macros.hpp>

namespace BS {

    TEST_CASE("TestSize","[Genome]") {
        
        Genome genome;
    
        CHECK(genome.size() == 0);
    }

    TEST_CASE("TestAdd","[Genome]") {
        
        Gene g = makeRandomGene();

        Genome genome;
        genome.add(g);

        CHECK(genome.size() == 1);
    }
    
    TEST_CASE("TestGeneAt","[Genome]") {
        
        Genome genome;

        for (int i = 0; i < 10; i++){
            genome.add(makeRandomGene());
        }

        // g.sourceType = NEURON;
        
        // genome.add(g);

        Gene &gene2 = genome.geneAt(0);
        // CHECK(gene3.sourceType == NEURON);

        gene2.sourceType = SENSOR;

        Gene &gene3 = genome.geneAt(0);
        CHECK(gene3.sourceType == SENSOR);
    }

    // TEST_CASE("TestEquality","[Genome]") {
        
    //     Gene g;
    //     g.sourceType = NEURON;
    //     g.sourceNum = 1;
    //     g.sinkType = ACTION;
    //     g.sinkNum = 5;
    //     g.weight = 56;

    //     Gene g2;
    //     g2.sourceType = NEURON;
    //     g2.sourceNum = 1;
    //     g2.sinkType = ACTION;
    //     g2.sinkNum = 5;
    //     g2.weight = 56;

    //     bool result = false;

    //     SECTION("Equals") {
    //         result = (g == g2);
    //         CHECK(result);
    //     }

    //     SECTION("Different sourceNum") {
    //         g2.sourceNum = 10;
    //         result = (g != g2);

    //         CHECK(result); 
    //     }

    //     SECTION("Different sourceType") {
    //         g2.sourceType = SENSOR;
    //         result = (g != g2);

    //         CHECK(result); 
    //     }

    //     SECTION("Different sinkType") {
    //         g2.sinkType = NEURON;
    //         result = (g != g2);

    //         CHECK(result); 
    //     }

    //     SECTION("Different sinkNum") {
    //         g2.sinkNum = 33;
    //         result = (g != g2);

    //         CHECK(result); 
    //     }

    //     SECTION("Different weight") {
    //         g2.weight = 101;
    //         result = (g != g2);

    //         CHECK(result); 
    //     }    
    // }
}