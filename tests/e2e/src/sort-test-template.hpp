/*
dont using import std, because include opencl
(no module global fragment in .cpp file)
*/

#if not defined(SORT_FUNCTION)
#error "Please define macro 'SORT_FUNCTION' before include this file"
#endif /* not defined(SORT_FUNCTION) */

#include <iostream>
#include <vector>
#include <stdexcept>

#include "global/macros.hpp"

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif /* CL_HPP_TARGET_OPENCL_VERSION */

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"

ON_STACKTRACE(import exception_stack_trace;)

int main()
try
{
    std::vector<int> v;

    int vi;
    while (std::cin >> vi)
        v.push_back(vi);

    SORT_FUNCTION(v.begin(), v.end());

    for (auto vii: v)
        std::cout << vii << " ";

    std::cout << std::endl;

    return EXIT_SUCCESS;
}
catch (cl::BuildError &err)
{
    std::cerr << "OCL BUILD ERROR: " << err.err() << ":" << err.what() << std::endl;

    std::cerr << "-- Log --\n";
    for (auto e : err.getBuildLog())
        std::cerr << e.second;
    std::cerr << "-- End log --\n";
ON_STACKTRACE(
    debug::stacktrace::show_exception_stacktrace();
) /* ON_STACKTRACE */
    return EXIT_FAILURE;
}
catch (cl::Error &err)
{
    std::cerr << "OCL ERROR: " << err.err() << " : " << err.what() << std::endl;
ON_STACKTRACE(
    debug::stacktrace::show_exception_stacktrace();
) /* ON_STACKTRACE */
    return EXIT_FAILURE;
}
catch (std::runtime_error &err)
{
    std::cerr << "RUNTIME ERROR: " << err.what() << std::endl;
ON_STACKTRACE(
    debug::stacktrace::show_exception_stacktrace();
) /* ON_STACKTRACE */
    return EXIT_FAILURE;
}
catch (...)
{
    std::cerr << "UNKNOWN ERROR\n";
ON_STACKTRACE(
    debug::stacktrace::show_exception_stacktrace();
) /* ON_STACKTRACE */
    return EXIT_FAILURE;
}
