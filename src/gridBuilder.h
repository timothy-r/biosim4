#pragma once

#include "grid.h"
#include "random.h"

namespace BS {

    /**
     *
    */
    class GridBuilder
    {
        public:
            
            GridBuilder(RandomUintGenerator &r);

            Grid createBarrier(Grid &g, unsigned barrierType);
        
        private:

            void drawBox(Grid &grid, int16_t minX, int16_t minY, int16_t maxX, int16_t maxY);
            
            RandomUintGenerator& randomUint;
    };
    
}