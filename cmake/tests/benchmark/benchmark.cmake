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
    )

    target_include_directories(${target}
      PRIVATE
        ${BENCHMARK_SRC_DIR}
        ${to_include}
    )

    target_compile_definitions(${target}
      PRIVATE
          $<$<BOOL:${CMAKE_CXX_MODULE_STD}>:BITONICSORT_CXX_23_SUPPORT>
    )
endfunction(create_benchmark target)

set(BITONICSORST_TIME_MEASURE bitonic-benchmark)
set(STDSORT_TIME_MEASURE stdsort-benchmark)

# =================================================================================================
create_benchmark("${BITONICSORST_TIME_MEASURE}" "bitonicsort-time-measure.cpp" "sort::bitonic" "")
create_benchmark("${STDSORT_TIME_MEASURE}" "stdsort-time-measure.cpp" "" "")

# =================================================================================================
# creating run_test from run_test.in

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
