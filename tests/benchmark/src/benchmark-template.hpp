#if not defined(SORT_FUNCTION)
#error "Define macro 'SORT_FUNCTION' before include this file"
#endif /* not defined(SORT_FUNCTION) */

#if defined(BITONICSORT_CXX_23_SUPPORT)
import std;
#else /* defined(BITONICSORT_CXX_23_SUPPORT) */
#include <vector>
#include <chrono>
#include <iostream>
#endif /* defined(BITONICSORT_CXX_23_SUPPORT) */

int main()
{
    std::vector<int> v;

    int vi;
    while (std::cin >> vi)
        v.push_back(vi);

    std::chrono::high_resolution_clock::time_point TimeStart = std::chrono::high_resolution_clock::now();

    SORT_FUNCTION(v.begin(), v.end());

    std::chrono::high_resolution_clock::time_point TimeFin = std::chrono::high_resolution_clock::now();

    long Dur = std::chrono::duration_cast<std::chrono::milliseconds>(TimeFin - TimeStart).count();
    std::cout << Dur << " ";

    return 0;
}
