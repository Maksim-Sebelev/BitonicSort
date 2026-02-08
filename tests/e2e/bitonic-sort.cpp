#include <iostream>
#include <vector>
#include <stdexcept>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/stacktrace.hpp>

#include <global/macros.hpp>

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include <CL/cl.h>
#include "CL/opencl.hpp"

#if defined(BITONICSORT_MODULES)
import bitonic_sort;
import exception_stack_trace;
#else /* defined(BITONICSORT_MODULES) */
#include "sort/bitonic/sort.hpp"
#endif /* defined(BITONICSORT_MODULES) */


int main()
try
{
    std::vector<int> v;

    int vi;
    while (std::cin >> vi)
        v.push_back(vi);

    sort::bitonic::sort(v.begin(), v.end());

    for (auto vii: v)
        std::cout << vii << " ";

    std::cout << std::endl;

    return 0;
}
catch (cl::BuildError &err)
{
    std::cerr << "OCL BUILD ERROR: " << err.err() << ":" << err.what() << std::endl;

    std::cerr << "-- Log --\n";
    for (auto e : err.getBuildLog())
        std::cerr << e.second;
    std::cerr << "-- End log --\n";

#if defined(BITONICSORT_MODULES)
    debug::stacktrace::show_exception_stacktrace();
#endif /* defined(BITONICSORT_MODULES) */

    return 1;
}
catch (cl::Error &err)
{
    std::cerr << "OCL ERROR: " << err.err() << " : " << err.what() << std::endl;
#if defined(BITONICSORT_MODULES)
    debug::stacktrace::show_exception_stacktrace();
#endif /* defined(BITONICSORT_MODULES) */
    return 1;
}
catch (std::runtime_error &err)
{
    std::cerr << "RUNTIME ERROR: " << err.what() << std::endl;
#if defined(BITONICSORT_MODULES)
    debug::stacktrace::show_exception_stacktrace();
#endif /* defined(BITONICSORT_MODULES) */
    return 1;
}
catch (...)
{
    std::cerr << "UNKNOWN ERROR\n";
#if defined(BITONICSORT_MODULES)
    debug::stacktrace::show_exception_stacktrace();
#endif /* defined(BITONICSORT_MODULES) */
    return 1;
}
