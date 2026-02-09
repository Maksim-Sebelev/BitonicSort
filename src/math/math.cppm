module;

//----------------------------------------------------------------------------------------------------------------------------

#include "global/macros.hpp"

//----------------------------------------------------------------------------------------------------------------------------

#if not defined(BITONICSORT_CXX_23_SUPPORT)
#include <type_traits>
#endif /* not defined(BITONICSORT_CXX_23_SUPPORT) */

//----------------------------------------------------------------------------------------------------------------------------

export module math_functions;

//----------------------------------------------------------------------------------------------------------------------------

#if defined(BITONICSORT_CXX_23_SUPPORT)
import std;
#endif /* defined(BITONICSORT_CXX_23_SUPPORT) */

//----------------------------------------------------------------------------------------------------------------------------

namespace math
{

export
template <typename T> requires std::is_integral_v<T>
T min_power_of_2_greater_or_equal_than(T x)
{
    if (x <= 0) return 1;

    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

} /* namespace math */