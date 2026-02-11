#pragma once

namespace math
{

template <typename T>
inline T get_min_natural_power_of_2_greater_or_equal_than(T x)
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
