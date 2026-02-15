
#pragma once

//----------------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <iterator>
#include <cstddef>
#include <string>
#include <limits>
#include <sstream>

//----------------------------------------------------------------------------------------------------------------------------

#include "global/macros.hpp"
#include "math/math.hpp"

//----------------------------------------------------------------------------------------------------------------------------

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif /* CL_HPP_TARGET_OPENCL_VERSION */

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "CL/opencl.hpp"

//----------------------------------------------------------------------------------------------------------------------------

#if not defined(BITONICSORT_OPENCL_KERNEL)
#error "Please define 'BITONICSORT_OPENCL_KERNEL' before include this header. Witout this macro we cannot find kernnel, because didnt know absolute way."
#endif /* not defined(BITONICSORT_OPENCL_KERNEL) */

//----------------------------------------------------------------------------------------------------------------------------

namespace sort
{
namespace bitonic
{

//----------------------------------------------------------------------------------------------------------------------------

template <typename It> inline void sort(It begin, It end);
template <typename It> inline void sort_local(It begin, It end);

//----------------------------------------------------------------------------------------------------------------------------

namespace __detail
{

//----------------------------------------------------------------------------------------------------------------------------

template <typename  T>
inline std::string get_type_name() { return "unknown"; }

// not a constexpr, because c++11, and we need std::string as return type
template <> inline std::string get_type_name<int>() { return STRINGIFY(int); }
template <> inline std::string get_type_name<float>() { return STRINGIFY(float); }
template <> inline std::string get_type_name<double>() { return STRINGIFY(double); }


//----------------------------------------------------------------------------------------------------------------------------

class OpenCLSorting
{
  private:
    cl::Platform platform_;
    cl::Context context_;
    cl::CommandQueue queue_;
    std::string kernel_;
    size_t local_size_;

    inline static cl::Platform select_platform()
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

    inline static cl::Context get_gpu_context(cl_platform_id PId)
    {
        cl_context_properties properties[] =
        {
        CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(PId),
        0 // signals end of property list
        };

        return cl::Context(CL_DEVICE_TYPE_GPU, properties);
    }

    inline static std::string readFile(const char *Path)
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

    using sort_small_blocks_t = cl::KernelFunctor<cl::Buffer>;
    using sort_big_blocks_t = cl::KernelFunctor<cl::Buffer, cl_uint>;
    using big_compare_distance_t = cl::KernelFunctor<cl::Buffer, cl_uint, cl_uint>;
    using small_compare_distance_t = cl::KernelFunctor<cl::Buffer, cl_uint>;

    inline big_compare_distance_t   get_big_compare_distance();
    inline small_compare_distance_t get_small_compare_distance();

    inline sort_small_blocks_t get_sorting_small_blocks();
    inline sort_big_blocks_t get_sorting_big_blocks();

    template <typename It>
    inline void add_type_define_in_kernel();
    template <typename It>
    inline cl::Buffer copy_input_on_queue(It begin, It end, size_t& cl_buf_size);
    
    enum { BUILD_KERNEL_IMMEDIATELY = true };
  public:

    OpenCLSorting() :
    platform_(select_platform()),
    context_(get_gpu_context(platform_())),
    queue_(context_, cl::QueueProperties::Profiling | cl::QueueProperties::OutOfOrder), 
    kernel_(readFile(BITONICSORT_OPENCL_KERNEL)),
    local_size_(256)
    {}

    template <typename It>
    inline void sort(It begin, It end);

    template <typename It>
    inline void sort_local(It begin, It end);
};

//----------------------------------------------------------------------------------------------------------------------------

template <typename It>
inline void OpenCLSorting::add_type_define_in_kernel()
{
    using type = typename It::value_type;

    kernel_ = "#define TYPE " + get_type_name<type>() + "\n" "#define LOCAL_SIZE " + std::to_string(local_size_) + "\n" + kernel_;
}

//----------------------------------------------------------------------------------------------------------------------------

template <typename It>
inline cl::Buffer OpenCLSorting::copy_input_on_queue(It begin, It end, size_t& cl_buf_size)
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

inline OpenCLSorting::big_compare_distance_t OpenCLSorting::get_big_compare_distance()
{
    cl::Program program(context_, kernel_, BUILD_KERNEL_IMMEDIATELY);
    return big_compare_distance_t{program, "big_compare_distance"};
}

//-----------------------------------------------------------------------------

inline OpenCLSorting::small_compare_distance_t OpenCLSorting::get_small_compare_distance()
{
    cl::Program program(context_, kernel_, BUILD_KERNEL_IMMEDIATELY);
    return small_compare_distance_t{program, "small_compare_distance"};
}

//-----------------------------------------------------------------------------

template <typename It>
void OpenCLSorting::sort_local(It begin, It end)
{
ON_TIME(
    cl_ulong GPUTimeStart;
    cl_ulong GPUTimeFin;
    unsigned long long gpu_time = 0;
) /* ON_TIME */

    using type = typename It::value_type;

    add_type_define_in_kernel<It>();

    size_t cl_buf_size;
    cl::Buffer cl_data = copy_input_on_queue(begin, end, cl_buf_size);

    sort_small_blocks_t small_blocks_sizes = get_sorting_small_blocks();
    big_compare_distance_t big_compare_distance = get_big_compare_distance();
    small_compare_distance_t small_compare_distance = get_small_compare_distance();

    cl::EnqueueArgs Args1(queue_, cl::NDRange(cl_buf_size), cl::NDRange(local_size_));
    cl::EnqueueArgs Args2(queue_, cl::NDRange(cl_buf_size));
    cl::EnqueueArgs Args3(queue_, cl::NDRange(cl_buf_size), cl::NDRange(local_size_));

    cl::Event Evt = small_blocks_sizes(Args1, cl_data);
    Evt.wait();
ON_TIME(
    GPUTimeStart = Evt.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    GPUTimeFin = Evt.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    gpu_time += (GPUTimeFin - GPUTimeStart);
) /* ON_TIME */

    for (cl_uint block_size = local_size_ << 1; block_size <= cl_buf_size; block_size <<= 1)
    {
        for (cl_uint stage_comparing_distance = (block_size >> 1); stage_comparing_distance >= local_size_; stage_comparing_distance >>= 1)
        {
            Evt = big_compare_distance(Args2, cl_data, block_size, stage_comparing_distance);
            Evt.wait();
ON_TIME(
    GPUTimeStart = Evt.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    GPUTimeFin = Evt.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    gpu_time += (GPUTimeFin - GPUTimeStart);
) /* ON_TIME */
        }

        Evt = small_compare_distance(Args3, cl_data, block_size);
        Evt.wait();
ON_TIME(
    GPUTimeStart = Evt.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    GPUTimeFin = Evt.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    gpu_time += (GPUTimeFin - GPUTimeStart);
) /* ON_TIME */
    }

    cl::copy(queue_, cl_data, begin, end);

ON_TIME(
    gpu_time /= 1000000;
    std::cout << "(GPU: " << gpu_time << ", ";
) /* ON_TIME */
}

//-----------------------------------------------------------------------------

template <typename It>
inline void OpenCLSorting::sort(It begin, It end)
{
ON_TIME(
    cl_ulong GPUTimeStart;
    cl_ulong GPUTimeFin;
    unsigned long long gpu_time = 0;
) /* ON_TIME */

    add_type_define_in_kernel<It>();

    size_t cl_buf_size;
    cl::Buffer cl_data = copy_input_on_queue(begin, end, cl_buf_size);

    sort_small_blocks_t sorting_small_blocks = get_sorting_small_blocks();

    cl::NDRange GlobalRange(cl_buf_size);
    cl::NDRange LocalRange(local_size_);

    cl::EnqueueArgs ArgsSmallBlocks(queue_, GlobalRange, LocalRange);
    cl::Event FirstSortingSteps = sorting_small_blocks(ArgsSmallBlocks, cl_data);
    FirstSortingSteps.wait();

ON_TIME(
    GPUTimeStart = FirstSortingSteps.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    GPUTimeFin = FirstSortingSteps.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    gpu_time += (GPUTimeFin - GPUTimeStart);
) /* ON_TIME */

    sort_big_blocks_t sorting_big_blocks = get_sorting_big_blocks();

    cl::EnqueueArgs ArgsBigBlocks(queue_, GlobalRange);

    cl::Event LastSortingSteps = sorting_big_blocks(ArgsBigBlocks, cl_data, cl_buf_size);
    LastSortingSteps.wait();

ON_TIME(
    GPUTimeStart = LastSortingSteps.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    GPUTimeFin = LastSortingSteps.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    gpu_time += (GPUTimeFin - GPUTimeStart);
) /* ON_TIME */

    cl::copy(queue_, cl_data, begin, end);

ON_TIME(
    gpu_time /= 1000000;
    std::cout << "(GPU: " << gpu_time << ", ";
) /* ON_TIME */
}

//-----------------------------------------------------------------------------

} /* namespace __detail */

//-----------------------------------------------------------------------------

template <typename It>
inline void sort(It begin, It end)
{
	__detail::OpenCLSorting{}.sort(begin, end);
}

//-----------------------------------------------------------------------------

template <typename It>
inline void sort_local(It begin, It end)
{
	__detail::OpenCLSorting{}.sort_local(begin, end);
}

//-----------------------------------------------------------------------------

} /* namespace bitonic */
} /* namespace sort */

//----------------------------------------------------------------------------------------------------------------------------
