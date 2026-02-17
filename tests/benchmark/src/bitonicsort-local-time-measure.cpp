#if defined(BITONICSORT_MODULES)
import bitonic_sort;
#else /* defined(BITONICSORT_MODULES) */
#include "sort/bitonic/sort.hpp"
#endif /* defined(BITONICSORT_MODULES) */

#define SORT_FUNCTION sort::bitonic::sort_local
#include "benchmark-template.hpp"
