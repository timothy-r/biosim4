#include "../../grid.h"
#include "../../gridBuilder.h"
#include "../../random.h"
#include "omp.h"

#include <catch2/catch_test_macros.hpp>


namespace BS {

TEST_CASE("TestBuildNoBarriers","[GridBuilder]") {
    
    RandomUintGenerator randomUint;
    randomUint.initialize(true, 12345678);

    Grid g = Grid();
    uint16_t sizeX = 12;
    uint16_t sizeY = 12;
    g.init(sizeX, sizeY);

    GridBuilder b = GridBuilder(randomUint);

    // 0 is no barriers
    b.createBarrier(g, 0);

    for (int16_t x = 0; x < sizeX; ++x) {
        for (int16_t y = 0; y < sizeY; ++y) {
            CHECK_FALSE(g.isBarrierAt({x, y}));
        }
    }
}

TEST_CASE("TestBuildVerticalBar","[GridBuilder]") {

    RandomUintGenerator randomUint;
    randomUint.initialize(true, 12345678);

    Grid g = Grid();
    uint16_t sizeX = 12;
    uint16_t sizeY = 12;
    g.init(sizeX, sizeY);

    GridBuilder b = GridBuilder(randomUint);

    // 1 is vertical bar
    b.createBarrier(g, 1);

    int16_t minX = g.sizeX() / 2; // 6
    int16_t maxX = minX + 1; // 7
    int16_t minY = g.sizeY() / 4; // 3
    int16_t maxY = minY + g.sizeY() / 2; // 3 + 12 / 2 = 9

    for (int16_t x = minX; x <= maxX; ++x) {
        for (int16_t y = minY; y < maxY; ++y) {
            CHECK(g.isBarrierAt({x, y}));
        }
    }

    CHECK_FALSE(g.getBarrierLocations().empty());
}

TEST_CASE("TestBuildRandomVerticalBar","[GridBuilder]") {
    RandomUintGenerator randomUint;
    randomUint.initialize(true, 12345678);

    Grid g = Grid();
    uint16_t sizeX = 128;
    uint16_t sizeY = 128;
    g.init(sizeX, sizeY);

    GridBuilder b = GridBuilder(randomUint);

    // 1 is random vertical bar
    b.createBarrier(g, 2);

    CHECK_FALSE(g.getBarrierLocations().empty());
}

}