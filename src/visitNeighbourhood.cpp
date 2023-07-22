// #include <utility>
// #include <cassert>
// #include <algorithm>
// #include <iostream>

// #include "../include/coord.h"

// namespace BS {

//     // This is a utility function used when inspecting a local neighborhood around
//     // some location. This function feeds each valid (in-bounds) location in the specified
//     // neighborhood to the specified function. Locations include self (center of the neighborhood).
//     void visitNeighborhood(Coord loc, float radius, std::function<void(Coord)> f)
//     {
//         for (int dx = -std::min<int>(radius, loc.x); dx <= std::min<int>(radius, (p.sizeX - loc.x) - 1); ++dx) {
//             int16_t x = loc.x + dx;
//             assert(x >= 0 && x < p.sizeX);
//             int extentY = (int)sqrt(radius * radius - dx * dx);
//             for (int dy = -std::min<int>(extentY, loc.y); dy <= std::min<int>(extentY, (p.sizeY - loc.y) - 1); ++dy) {
//                 int16_t y = loc.y + dy;
//                 assert(y >= 0 && y < p.sizeY);
//                 f( Coord { x, y} );
//             }
//         }
//     }

// }