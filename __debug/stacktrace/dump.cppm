module;

#include <iostream>

#include <boost/stacktrace.hpp>
#include <boost/stacktrace/stacktrace.hpp>

#include "global/macros.hpp"

export module exception_stack_trace;

namespace debug
{
namespace stacktrace
{

export template <bool do_flush = false> void show_exception_stacktrace(std::ostream &os = std::cerr);
export template <bool do_flush = false> void show_stacktrace(std::ostream &os = std::cerr);

bool is_main(std::string_view function_name)
{
    return 
        function_name == "main" ||
        function_name == "__libc_start_main";
}

std::ostream &operator<<(std::ostream &os, const boost::stacktrace::stacktrace &trace)
{
    for (std::size_t it = 0, ite = trace.size(); it < ite; ++it)
    {
        os << " " << it << "#" << " " << trace[it] << "\n";
        if (is_main(trace[it].name()))
            break;
    }

    return os;
}

template <bool do_flush> void show_exception_stacktrace(std::ostream &os)
{
    auto trace = boost::stacktrace::stacktrace::from_current_exception();
    os << "exception stacktrace:\n" << trace;

    if constexpr (not do_flush)
        return;

    os << std::flush;
};

template <bool do_flush> void show_stacktrace(std::ostream &os)
{
    auto trace = boost::stacktrace::stacktrace();
    os << "stacktrace:\n" << trace;

    if constexpr (not do_flush)
        return;

    os << std::flush;
};

} /* namespace stacktrace */
} /* namespace debug */