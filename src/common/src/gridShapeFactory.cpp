

#include "../include/gridShapeFactory.h"
#include "../include/gridCircle.h"

namespace BS
{
    std::unique_ptr<GridShape> GridShapeFactory::createGridCircle(Grid &g, Coord l, float r)
    {
        return std::make_unique<GridCircle>(g, l, r);
    }

} // namespace BS
