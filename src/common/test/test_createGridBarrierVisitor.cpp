#include "../../createGridBarrierVisitor.h"
#include "../../grid.h"
#include "../include/coord.h"

#include <catch2/catch_test_macros.hpp>

namespace BS {

    TEST_CASE("TestVisit","[CreateGridBarrierVisitor]") {
        
        Grid g = Grid();
        uint16_t numLayers = 1;
        uint16_t sizeX = 128;
        uint16_t sizeY = 128;
        g.init(numLayers, sizeX, sizeY);

        CreateGridBarrierVisitor visitor = CreateGridBarrierVisitor(g);
        
        Coord loc = Coord(64, 64);

        CHECK_FALSE(g.isBarrierAt(loc));

        float radius = 5.0;
        g.acceptCircular(visitor, loc, radius);

        CHECK(g.isBarrierAt(loc));

    }

    TEST_CASE("TestVisitRespectsGridBounds","[CreateGridBarrierVisitor]") {
        
        Grid g = Grid();
        uint16_t numLayers = 1;
        uint16_t sizeX = 128;
        uint16_t sizeY = 128;
        g.init(numLayers, sizeX, sizeY);


        CreateGridBarrierVisitor visitor = CreateGridBarrierVisitor(g);
        
        // loc is out of the grid boundaries
        Coord loc = Coord(200, 200);

        CHECK_FALSE(g.isBarrierAt(loc));

        float radius = 5.0;
        g.acceptCircular(visitor, loc, radius);

        CHECK_FALSE(g.isBarrierAt(loc));

    }
}