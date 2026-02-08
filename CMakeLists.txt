cmake_minimum_required(VERSION 3.30)

project(BitonicSort
  VERSION 1.1
  LANGUAGES CXX
)

# ==================================================================================

if (CMAKE_EXPERIMENTAL_CXX_IMPORT_STD)
  set(CMAKE_CXX_STANDARD 23)
  set(CMAKE_CXX_MODULE_STD ON)

  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()

else()
  set(CMAKE_CXX_STANDARD 20)
endif()

# ==================================================================================
# set some global variables

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/include)
set(DEBUG_DIR ${CMAKE_SOURCE_DIR}/__debug)
set(CMAKE_SETTINGS_DIR ${CMAKE_SOURCE_DIR}/cmake)

string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")

# ==================================================================================
# add math library

set(MATH_LIB math)
set(MATH_SRC_DIR ${SRC_DIR}/math)

set(MATH_SRCS
    ${MATH_SRC_DIR}/math.cppm
)

add_library(${MATH_LIB})

target_sources(${MATH_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
      ${MATH_SRCS}
)

target_include_directories(${MATH_LIB}
  PRIVATE
    ${INC_DIR}
)

target_compile_definitions(${MATH_LIB}
  PRIVATE
    $<$<BOOL:${CMAKE_CXX_MODULE_STD}>:BITONICSORT_CXX_23_SUPPORT>
)

# ==================================================================================
# add bitonic sort library

set(SORT_SRC_DIR ${SRC_DIR}/sort)

set(BITONICSORT_LIB bitonic)
set(BITONICSORT_LIB ${BITONICSORT_LIB})
set(BITONICSORT_SRC_DIR ${SORT_SRC_DIR}/bitonic)

set(BITONICSORT_SRCS
    ${BITONICSORT_SRC_DIR}/sort.cppm
)

set(BITONICSORT_OPENCL_KERNEL ${BITONICSORT_SRC_DIR}/sort.cl)

add_library(${BITONICSORT_LIB})

target_sources(${BITONICSORT_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
      ${BITONICSORT_SRCS}
)

target_link_libraries(${BITONICSORT_LIB}
  PUBLIC
    OpenCL
    ${MATH_LIB}
)

target_include_directories(${BITONICSORT_LIB}
  PRIVATE
    ${INC_DIR}
)

target_compile_definitions(${BITONICSORT_LIB}
  PRIVATE
    $<$<BOOL:${CMAKE_CXX_MODULE_STD}>:BITONICSORT_CXX_23_SUPPORT>
    BITONICSORT_OPENCL_KERNEL="${BITONICSORT_OPENCL_KERNEL}"
)

# ==================================================================================

set(BITONICSORT_NAMESPACE sort)

# ==================================================================================

set(TEST_DIR ${CMAKE_SOURCE_DIR}/tests)

# ==================================================================================
# install like library if enabled

if (INSTALL)
  message(${INSTALL})
  include(${CMAKE_SETTINGS_DIR}/install/install-bitonicsort.cmake)
endif()

# ==================================================================================
# building tests if enabled

if (BUILD_TESTS)
  include(${CMAKE_SETTINGS_DIR}/tests/e2e/bitonic-sort-e2e-tests.cmake)
endif()

# ==================================================================================
# building benchmark if enabled

if (BENCHMARK)
  include(${CMAKE_SETTINGS_DIR}/tests/benchmark/benchmark.cmake)
endif()

# ==================================================================================
