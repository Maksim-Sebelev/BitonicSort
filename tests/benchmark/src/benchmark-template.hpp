#if not defined(SORT_FUNCTION)
#error "Define macro 'SORT_FUNCTION' before include this file"
#endif

#if defined(BITONICSORT_CXX_23_SUPPORT)
import std;
#else /* defined(BITONICSORT_CXX_23_SUPPORT) */
#include <vector>
#include <chrono>
#include <iostream>
#endif /* defined(BITONICSORT_CXX_23_SUPPORT) */

int main()
{
    std::chrono::high_resolution_clock::time_point TimeStart, TimeFin;
    long Dur;

    std::vector<int> v;

    int vi;
    while (std::cin >> vi)
        v.push_back(vi);

    TimeStart = std::chrono::high_resolution_clock::now();

    SORT_FUNCTION(v.begin(), v.end());

    TimeFin = std::chrono::high_resolution_clock::now();

    Dur = std::chrono::duration_cast<std::chrono::milliseconds>(TimeFin - TimeStart).count();
    std::cout << Dur << " ";

    return 0;
}
