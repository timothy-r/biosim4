#pragma once

#include "common/include/coord.h"
#include "common/include/dir.h"

namespace BS
{
    float getPopulationDensityAlongAxis(Coord loc, Dir dir);
    float getSignalDensity(unsigned layerNum, Coord loc);
    float getSignalDensityAlongAxis(unsigned layerNum, Coord loc, Dir dir);    

} // namespace BS
