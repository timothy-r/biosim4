#include "gridBuilder.h"
#include "random.h"
#include "createGridBarrierVisitor.h"

namespace BS {

    GridBuilder::GridBuilder(RandomUintGenerator &r) : randomUint(r) {} 

    /**
     * two use cases, create a new Grid, refresh an existing Grid
     * reset grid & barriers, then add new barriers
    */
    Grid GridBuilder::createBarrier(Grid &g, unsigned barrierType)
    {
        g.zeroFill();

        switch(barrierType) {

            case 0:
                return g;

            // Vertical bar in constant location
            case 1:
            {
                int16_t minX = g.sizeX() / 2;
                int16_t maxX = minX + 1;
                int16_t minY = g.sizeY() / 4;
                int16_t maxY = minY + g.sizeY() / 2;
                drawBox(g, minX, minY, maxX, maxY);
            }
               break;

            // Vertical bar in random location
            // must respect size of grid, change hard coded 20 value
            // calculate from grid size x & y
            case 2:
            {
                int16_t minX = randomUint(20, g.sizeX() - 20);
                int16_t maxX = minX + 1;
                int16_t minY = randomUint(20, g.sizeY() / 2 - 20);
                int16_t maxY = minY + g.sizeY() / 2;

                drawBox(g, minX, minY, maxX, maxY);
            }
                break;

            // five blocks staggered
            case 3:
            {
                int16_t blockSizeX = 2;
                int16_t blockSizeY = g.sizeX() / 3;

                int16_t x0 = g.sizeX() / 4 - blockSizeX / 2;
                int16_t y0 = g.sizeY() / 4 - blockSizeY / 2;
                int16_t x1 = x0 + blockSizeX;
                int16_t y1 = y0 + blockSizeY;

                drawBox(g, x0, y0, x1, y1);

                x0 += g.sizeX() / 2;
                x1 = x0 + blockSizeX;
                drawBox(g, x0, y0, x1, y1);
                
                y0 += g.sizeY() / 2;
                y1 = y0 + blockSizeY;
                drawBox(g, x0, y0, x1, y1);
                
                x0 -= g.sizeX() / 2;
                x1 = x0 + blockSizeX;
                drawBox(g, x0, y0, x1, y1);
                
                x0 = g.sizeX() / 2 - blockSizeX / 2;
                x1 = x0 + blockSizeX;
                y0 = g.sizeY() / 2 - blockSizeY / 2;
                y1 = y0 + blockSizeY;
                drawBox(g, x0, y0, x1, y1);
            }
                break;
            

            // Horizontal bar in constant location
            case 4:
            {
                int16_t minX = g.sizeX() / 4;
                int16_t maxX = minX + g.sizeX() / 2;
                int16_t minY = g.sizeY() / 2 + g.sizeY() / 4;
                int16_t maxY = minY + 2;
                drawBox(g, minX, minY, maxX, maxY);
            }
                break;

            // Three floating islands -- different locations every generation
            // case 5:
            //     break;

            // Spots, specified number, radius, locations
            case 6:
            {
                // parameterize these values
                unsigned numberOfLocations = 5;
                float radius = 5.0;

                // CreateGridBarrierVisitor visitor = CreateGridBarrierVisitor(g);

            // auto f = [&](Coord loc) {
            //     setBarrier(loc);
            //     // grid.set(loc, BARRIER);
            //     // barrierLocations.push_back(loc);
            // };

                unsigned verticalSliceSize = g.sizeY() / (numberOfLocations + 1);

                for (unsigned n = 1; n <= numberOfLocations; ++n) {
                    Coord loc = { (int16_t)(g.sizeX() / 2),
                                (int16_t)(n * verticalSliceSize) };

                    // visitor.acceptCircular(loc, radius);
                    
                    // visitNeighborhood(loc, radius, f);
                    // barrierCenters.push_back(loc);
                }

            }

            break;
            // default:
                // assert(false);

        }
        
        return g;
    }

    /**
     * 
    */
    void GridBuilder::drawBox(Grid &grid, int16_t minX, int16_t minY, int16_t maxX, int16_t maxY)
    {
        for (int16_t x = minX; x <= maxX; ++x) {
            for (int16_t y = minY; y <= maxY; ++y) {
                grid.setBarrier(x,y);
            }
        }
    }
}