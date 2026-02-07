module;

//----------------------------------------------------------------------------------------------------------------------------

#include "global/macros.hpp"

//----------------------------------------------------------------------------------------------------------------------------

#if not defined(BITONICSORT_CXX_23_SUPPORT)
#include <iterator>
#include <cstddef>
#include <fstream>
#include <string>
#include <limits>
#include <sstream>
#include <concepts>
#endif /* not defined(BITONICSORT_CXX_23_SUPPORT) */

//----------------------------------------------------------------------------------------------------------------------------

#if not defined(BITONICSORT_OPENCL_KERNEL)
#error "Please defined 'BITONICSORT_OPENCL_KERNEL' for this module. witout this macro we cannot find kernnel, because didnt know absolute way."
#else /* not defined(BITONICSORT_OPENCL_KERNEL) */
#define S_BITONICSORT_OPENCL_KERNEL BITONICSORT_OPENCL_KERNEL
#endif /* not defined(BITONICSORT_OPENCL_KERNEL) */

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"

//----------------------------------------------------------------------------------------------------------------------------

export module bitonic_sort;

//----------------------------------------------------------------------------------------------------------------------------

#if defined(BITONICSORT_CXX_23_SUPPORT)
import std;
#endif /* defined(BITONICSORT_CXX_23_SUPPORT) */

import math_functions;

//----------------------------------------------------------------------------------------------------------------------------

namespace sort
{
namespace bitonic
{

//----------------------------------------------------------------------------------------------------------------------------

export
template<typename T>
concept BitonicSortSupportedType = 
    std::same_as<T, int> || 
    std::same_as<T, float> || 
    std::same_as<T, double>;

//----------------------------------------------------------------------------------------------------------------------------

export template <typename It>
concept BitonicSortIteratorConcept = 
    std::random_access_iterator<It> && 
    BitonicSortSupportedType<typename It::value_type>;

//----------------------------------------------------------------------------------------------------------------------------

export template <BitonicSortIteratorConcept It> void sort(It begin, It end);

//----------------------------------------------------------------------------------------------------------------------------

class OpenCLSorting
{
  private:
    cl::Platform platform_;
    cl::Context context_;
    cl::CommandQueue queue_;
    std::string kernel_;

    static cl::Platform select_platform()
    {
        cl::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        for (auto p : platforms)
        {
            cl_uint numdevices = 0;
            ::clGetDeviceIDs(p(), CL_DEVICE_TYPE_GPU, 0, NULL, &numdevices);
            if (numdevices > 0)
                return cl::Platform(p);
        }
        throw std::runtime_error("No platform selected");
    }

    static cl::Context get_gpu_context(cl_platform_id PId)
    {
        cl_context_properties properties[] =
        {
        CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(PId),
        0 // signals end of property list
        };

        return cl::Context(CL_DEVICE_TYPE_GPU, properties);
    }

    static std::string readFile(const char *Path)
    {
        std::string Code;
        std::ifstream ShaderFile;
        ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        ShaderFile.open(Path);
        std::stringstream ShaderStream;
        ShaderStream << ShaderFile.rdbuf();
        ShaderFile.close();
        Code = ShaderStream.str();
        return Code;
    }

    template <BitonicSortSupportedType T>
    static constexpr std::string get_type_name()
    {
        if constexpr (std::same_as<T, int>) return "int";
        else if constexpr (std::same_as<T, float>) return "float";
        else if constexpr (std::same_as<T, double>) return "double";

        return "unknown";
    }

    using sort_t = cl::KernelFunctor<cl::Buffer, cl_uint, cl_uint, cl_uint>;
    
    template <BitonicSortIteratorConcept It>
    void add_type_define_in_kernel();
    template <BitonicSortIteratorConcept It>
    cl::Buffer copy_input_on_queue(It begin, It end, size_t& cl_buf_size);
    
    sort_t get_gpu_part_of_sort_function();

    enum { BUILD_KERNEL_IMMEDIATELY = true };
  public:

    OpenCLSorting() :
    platform_(select_platform()),
    context_(get_gpu_context(platform_())),
    queue_(context_, cl::QueueProperties::Profiling | cl::QueueProperties::OutOfOrder), 
    kernel_(readFile(S_BITONICSORT_OPENCL_KERNEL))
    {}

    template <BitonicSortIteratorConcept It>
    void sort(It begin, It end);
};

//----------------------------------------------------------------------------------------------------------------------------

template <BitonicSortIteratorConcept It>
void sort(It begin, It end)
{
	OpenCLSorting{}.sort(begin, end);
}

//----------------------------------------------------------------------------------------------------------------------------

template <BitonicSortIteratorConcept It>
void OpenCLSorting::add_type_define_in_kernel()
{
    using type = typename It::value_type;

    if constexpr ( std::same_as<type, int>)
        return;

    kernel_ = "#define TYPE " + get_type_name<type>() + "\n" + kernel_;
}

//----------------------------------------------------------------------------------------------------------------------------

template <BitonicSortIteratorConcept It>
cl::Buffer OpenCLSorting::copy_input_on_queue(It begin, It end, size_t& cl_buf_size)
{
    using type = typename It::value_type;

    const size_t size = std::distance(begin, end);
    cl_buf_size = math::min_power_of_2_greater_or_equal_than(size);

    cl::Buffer cl_data(context_, CL_MEM_READ_WRITE, cl_buf_size * sizeof(type));
    cl::copy(begin, end, cl_data);

    const size_t diff_between_real_size_and_cl_buf_size = cl_buf_size - size;

    if (diff_between_real_size_and_cl_buf_size > 0)
    {
        type fill_value = std::numeric_limits<type>::max();
        queue_.enqueueFillBuffer(cl_data, fill_value, size * sizeof(type), (diff_between_real_size_and_cl_buf_size) * sizeof(type));
    }

    return cl_data;
}

//-----------------------------------------------------------------------------

OpenCLSorting::sort_t OpenCLSorting::get_gpu_part_of_sort_function()
{
    cl::Program program(context_, kernel_, BUILD_KERNEL_IMMEDIATELY);
    return sort_t{program, "bitonic_sort_gpu_part"};
}

//-----------------------------------------------------------------------------

template <BitonicSortIteratorConcept It>
void OpenCLSorting::sort(It begin, It end)
{
ON_TIME(
    std::chrono::high_resolution_clock::time_point TimeStart, TimeFin;
    cl_ulong GPUTimeStart, GPUTimeFin;
    unsigned long long summary_gpu_time = 0;
    long Dur, GDur;

    TimeStart = std::chrono::high_resolution_clock::now();
) /* ON_TIME */

    add_type_define_in_kernel<It>();

    size_t cl_buf_size;
    cl::Buffer cl_data = copy_input_on_queue(begin, end, cl_buf_size);
    sort_t gpu_part_of_sort = get_gpu_part_of_sort_function();

	cl::NDRange GlobalRange(cl_buf_size);
    cl::EnqueueArgs Args(queue_, GlobalRange);

    for (size_t block_size = 2; block_size <= cl_buf_size; block_size <<= 1)
    {
        for (size_t compare_distance_bit_mask = (block_size >> 1); compare_distance_bit_mask > 0; compare_distance_bit_mask >>= 1)
        {
            cl::Event Evt = gpu_part_of_sort(Args, cl_data, cl_buf_size, block_size, compare_distance_bit_mask);
            Evt.wait();

ON_TIME(
            GPUTimeStart = Evt.getProfilingInfo<CL_PROFILING_COMMAND_START>();
            GPUTimeFin = Evt.getProfilingInfo<CL_PROFILING_COMMAND_END>();
            summary_gpu_time += (GPUTimeFin - GPUTimeStart);
) /* ON_TIME */
        }
    }

    cl::copy(queue_, cl_data, begin, end);

ON_TIME(
    GDur = summary_gpu_time / 1000000; // ns -> ms
    std::cout << "GPU pure time measured: " << GDur << " ms" << std::endl;

    TimeFin = std::chrono::high_resolution_clock::now();
    Dur = std::chrono::duration_cast<std::chrono::milliseconds>(TimeFin - TimeStart).count();
    std::cout << "GPU wall time measured: " << Dur << " ms" << std::endl;
) /* ON_TIME */
}

//-----------------------------------------------------------------------------

} /* namespace bitonic */
} /* namespace sort */

//----------------------------------------------------------------------------------------------------------------------------
