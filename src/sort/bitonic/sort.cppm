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
    size_t local_size_;

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
    static constexpr std::string get_type_name() { return "unknown"; }

    template <> constexpr std::string get_type_name<int>() { return STRINGIFY(int); }
    template <> constexpr std::string get_type_name<float>() { return STRINGIFY(float); }
    template <> constexpr std::string get_type_name<double>() { return STRINGIFY(double); }

    using sort_small_blocks_t = cl::KernelFunctor<cl::Buffer>;
    using sort_big_blocks_t = cl::KernelFunctor<cl::Buffer, cl_uint>;

    sort_small_blocks_t get_sorting_small_blocks();
    sort_big_blocks_t get_sorting_big_blocks();

    template <BitonicSortIteratorConcept It>
    void add_type_define_in_kernel();
    template <BitonicSortIteratorConcept It>
    cl::Buffer copy_input_on_queue(It begin, It end, size_t& cl_buf_size);
    

    enum { BUILD_KERNEL_IMMEDIATELY = true };

  public:

    OpenCLSorting() :
    platform_(select_platform()),
    context_(get_gpu_context(platform_())),
    queue_(context_, cl::QueueProperties::Profiling | cl::QueueProperties::OutOfOrder),
    kernel_(readFile(BITONICSORT_OPENCL_KERNEL)),
    local_size_(256)
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

    std::string defines;
    if constexpr (not std::is_same_v<type, int>) // int - default kernel value
        defines += "#define TYPE " + get_type_name<type>() + "\n";

    if (local_size_ != 256) // 256 - default kernel value
        defines += "#define LOCAL_SIZE " + std::to_string(local_size_) + "\n";

    if (defines.empty()) return;

    kernel_ = defines + kernel_;
}

//----------------------------------------------------------------------------------------------------------------------------

template <BitonicSortIteratorConcept It>
cl::Buffer OpenCLSorting::copy_input_on_queue(It begin, It end, size_t& cl_buf_size)
{
    using type = typename It::value_type;

    const size_t size = std::distance(begin, end);
    cl_buf_size = std::max(math::min_power_of_2_greater_or_equal_than(size), local_size_);

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

OpenCLSorting::sort_small_blocks_t OpenCLSorting::get_sorting_small_blocks()
{
    cl::Program program(context_, kernel_, BUILD_KERNEL_IMMEDIATELY);
    return sort_small_blocks_t{program, "small_blocks_sizes"};
}

//-----------------------------------------------------------------------------

OpenCLSorting::sort_big_blocks_t OpenCLSorting::get_sorting_big_blocks()
{
    cl::Program program(context_, kernel_, BUILD_KERNEL_IMMEDIATELY);
    return sort_big_blocks_t{program, "big_block_sizes"};
}

//-----------------------------------------------------------------------------

template <BitonicSortIteratorConcept It>
void OpenCLSorting::sort(It begin, It end)
{
    add_type_define_in_kernel<It>();

    size_t cl_buf_size;
    cl::Buffer cl_data = copy_input_on_queue(begin, end, cl_buf_size);
 
    sort_small_blocks_t sorting_small_blocks = get_sorting_small_blocks();

    cl::NDRange GlobalRange(cl_buf_size);
    cl::NDRange LocalRange(local_size_);

    cl::EnqueueArgs ArgsSmallBlocks(queue_, GlobalRange, LocalRange);
    cl::Event FirstSortingSteps = sorting_small_blocks(ArgsSmallBlocks, cl_data);
    FirstSortingSteps.wait();

    sort_big_blocks_t sorting_big_blocks = get_sorting_big_blocks();

    cl::EnqueueArgs ArgsBigBlocks(queue_, GlobalRange);
    cl::Event LastSortingSteps = sorting_big_blocks(ArgsBigBlocks, cl_data, cl_buf_size);
    LastSortingSteps.wait();

    cl::copy(queue_, cl_data, begin, end);

ON_TIME(
    cl_ulong GPUTimeStart = Evt.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    ck_ulong GPUTimeFin = Evt.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    GDur = (GPUTimeFin - GPUTimeStart) / 1000000; // ns -> ms
    std::cout << "GPU pure time measured: " << GDur << " ms" << std::endl;
) /* ON_TIME */
}

//-----------------------------------------------------------------------------

} /* namespace bitonic */
} /* namespace sort */

//----------------------------------------------------------------------------------------------------------------------------
