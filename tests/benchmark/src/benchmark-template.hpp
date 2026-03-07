#if not defined(SORT_FUNCTION)
#error "Define macro 'SORT_FUNCTION' before include this file"
#endif /* not defined(SORT_FUNCTION) */

#include <iostream>
#include <vector>
#include <chrono>
#include <stdexcept>

#include "global/macros.hpp"

#if not defined(CL_HPP_TARGET_OPENCL_VERSION)
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif /* not defined(CL_HPP_TARGET_OPENCL_VERSION) */

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"

ON_STACKTRACE(import exception_stack_trace;)

auto main() -> int
try
{
    std::vector<int> v;

    int vi;
    while (std::cin >> vi)
        v.emplace_back(vi);

    std::chrono::high_resolution_clock::time_point TimeStart = std::chrono::high_resolution_clock::now();

    SORT_FUNCTION(v.begin(), v.end());

    std::chrono::high_resolution_clock::time_point TimeFin = std::chrono::high_resolution_clock::now();

    unsigned long sort_time = std::chrono::duration_cast<std::chrono::milliseconds>(TimeFin - TimeStart).count();
    std::cout ON_TIME(<< "CPU: ") << sort_time << ON_TIME(")") " ";

    return EXIT_SUCCESS;
}
catch (cl::BuildError &err)
{
    std::cerr << "OpenCL build erros: " << err.err() << ":" << err.what() << std::endl;

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
    std::cerr << "OpenCL exception: " << err.err() << " : " << err.what() << std::endl;
ON_STACKTRACE(
    debug::stacktrace::show_exception_stacktrace();
) /* ON_STACKTRACE */
    return EXIT_FAILURE;
}
catch (std::runtime_error &err)
{
    std::cerr << "Run time error: " << err.what() << std::endl;
ON_STACKTRACE(
    debug::stacktrace::show_exception_stacktrace();
) /* ON_STACKTRACE */
    return EXIT_FAILURE;
}
catch (...)
{
    std::cerr << "Unknown exception\n";
ON_STACKTRACE(
    debug::stacktrace::show_exception_stacktrace();
) /* ON_STACKTRACE */
    return EXIT_FAILURE;
}
