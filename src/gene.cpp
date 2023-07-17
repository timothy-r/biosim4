#include "genome-neurons.h"
#include "random.h"

namespace BS
{
    
    float Gene::weightAsFloat() const 
    { 
        return weight / 8192.0; 
    }

    int16_t Gene::makeRandomWeight()
    { 
        return randomUint(0, 0xffff) - 0x8000; 
    }

} // namespace BS
