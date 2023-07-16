#include "dirFactory.h"
#include "common/include/compass.h"
#include "random.h"

namespace BS {
    
    /**
     * TODO: Inject the randomUint instance into this class
     * remove its dependence on globals
    */
    Dir DirFactory::random8()
    {
        return Dir(Compass::N).rotate(randomUint(0, 7)); 
    }


} // namespace 
