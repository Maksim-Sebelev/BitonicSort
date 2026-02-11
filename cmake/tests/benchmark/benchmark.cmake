set(BENCHMARK_DIR ${TEST_DIR}/benchmark)

set(BENCHMARK_SRC_DIR ${BENCHMARK_DIR}/src)
set(BENCHMARK_DAT_DIR ${BENCHMARK_DIR}/dat)

function(create_benchmark target source to_link to_include)
    add_executable(${target}
      ${BENCHMARK_SRC_DIR}/${source}
    )
    target_link_libraries(${target}
      PRIVATE
        ${to_link}
        # $<$<BOOL:${BITONICSORT_HEADER_ONLY}>:OpenCL::OpenCL>
    )

    target_include_directories(${target}
      PRIVATE
        ${BENCHMARK_SRC_DIR}
        ${INC_DIR}
        ${to_include}
    )

    target_compile_definitions(${target}
      PRIVATE
        $<$<BOOL:${BITONICSORT_MODULES}>:BITONICSORT_MODULES>
        $<$<BOOL:${CMAKE_CXX_MODULE_STD}>:BITONICSORT_CXX_23_SUPPORT>
        BITONICSORT_OPENCL_KERNEL="${SRC_DIR}/sort/bitonic/sort.cl"
    )
endfunction(create_benchmark target)

set(BITONICSORST_TIME_MEASURE bitonic-benchmark)
set(STDSORT_TIME_MEASURE stdsort-benchmark)
set(BITONICSORT_LOCAL_TIME_MEASURE bitonic-sort-local-benchmark)

# =================================================================================================

if (BITONICSORT_MODULES)
  create_benchmark("${BITONICSORST_TIME_MEASURE}" "bitonicsort-time-measure.cpp" "${BITONICSORT_LIB}" "")
  create_benchmark("${BITONICSORT_LOCAL_TIME_MEASURE}" "bitonicsort-local-time-measure.cpp" "${BITONICSORT_LIB}" "")
else()
  create_benchmark("${BITONICSORST_TIME_MEASURE}" "bitonicsort-time-measure.cpp" "OpenCL::OpenCL" "")
endif()

create_benchmark("${STDSORT_TIME_MEASURE}" "stdsort-time-measure.cpp" "" "")

# =================================================================================================
# creating run_test from run_test.sh.in

set(RUN_BENCHMARK_SCRIPT_IN ${BENCHMARK_SRC_DIR}/run-benchmark.sh.in)
set(RUN_BENCHMARK_OUTPUT_SCRIPT ${PROJECT_BINARY_DIR}/run-benchmark)

configure_file(
    ${RUN_BENCHMARK_SCRIPT_IN}
    ${RUN_BENCHMARK_OUTPUT_SCRIPT}
    @ONLY
)

# give execute permission to run_test
file(CHMOD ${RUN_BENCHMARK_OUTPUT_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

# =================================================================================================
