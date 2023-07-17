#pragma once

#include "grid.h"
#include "coord.h"

namespace BS
{   
    /**
     * interface for visitor classes that visit locations on a Grid
    */
    class GridLocationVisitor {
        public:
            // create sub classes with the Grid instance
            virtual void visit(Coord loc) = 0;
    };

} // namespace name
