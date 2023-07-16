#pragma once

#include "gridShape.h"
#include "coord.h"
#include "../../grid.h"

namespace BS
{
    class GridShapeFactory
    {
        private:
            /* data */

        public:
            static GridShape createGridCircle(Grid &g, Coord l, float r);

            static GridShape createGridSquare(Grid &g, Coord min, Coord max);


        // GridShapeFactory(/* args */);
        // GridShapeFactory();
    };
    
} // namespace BS
