message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(OpenCLICDLoader)
find_package(OpenCLHeadersCpp)
find_package(OpenCLHeaders)

set(CONANDEPS_LEGACY  OpenCL::OpenCL  OpenCL::HeadersCpp  OpenCL::Headers )