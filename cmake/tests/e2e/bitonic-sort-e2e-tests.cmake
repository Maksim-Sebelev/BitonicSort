# for debug exceptions
include(${CMAKE_SETTINGS_DIR}/debug/stacktrace.cmake)

# =================================================================================================
# add ctest

enable_testing()
include(CTest)

# =================================================================================================
# check thah pyhon3 exist.
# python need for e2e tests

find_program(PYTHON_EXECUTABLE
    NAMES python3 python
    DOC "Python executable"
)

if (NOT PYTHON_EXECUTABLE)
    message(FATAL_ERROR "Python3 NOT find. No test available.")
endif()

# =================================================================================================
# create tun_test script for e2e paracl test

set(E2E_DIR ${TEST_DIR}/e2e)
set(E2E_DAT_DIR ${E2E_DIR}/dat)

set(PYTHON_RUN_TEST_SCRIPT         ${E2E_DIR}/check-test-result.py)
set(RUN_TEST_SCRIPT_IN             ${E2E_DIR}/run_n_test.sh.in)
set(E2E_OUTPUT_SCRIPT              ${PROJECT_BINARY_DIR}/run_n_test)

# =================================================================================================
# compile bitonicsort runner
set(BITONICSORT_RUNNER bitonic-sort)

add_executable(${BITONICSORT_RUNNER}
    ${E2E_DIR}/bitonic-sort.cpp
)

target_link_libraries(${BITONICSORT_RUNNER}
  PRIVATE
    ${BITONICSORT_LIB}
    ${STACKTRACE_LIB}
)

target_include_directories(${BITONICSORT_RUNNER}
  PRIVATE
    ${INC_DIR}  
)

if (BITONICSORT_SHOW_EXCEPTION_STACKTRACE)
add_target_stacktrace_dump_lib(${BITONICSORT_RUNNER})
endif()

# =================================================================================================
# give execute permission to python script
file(CHMOD ${PYTHON_RUN_TEST_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

# =================================================================================================
# creating run_test from run_test.in
configure_file(
    ${RUN_TEST_SCRIPT_IN}
    ${E2E_OUTPUT_SCRIPT}
    @ONLY
)

# =================================================================================================
# give execute permission to run_test

file(CHMOD ${E2E_OUTPUT_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

# =================================================================================================

# function for adding tests (not unit) to some executable files
function(target_e2e_tests test_target run_script test_dir)

    file(GLOB test_files "${test_dir}/*.dat") # save all tests -files in variable 'test_files'

    # get quantity of tests
    list(LENGTH test_files num_tests)

    # check that at least 1 test was found
    if(${num_tests} EQUAL 0)
        message(WARNING "No dat files found in ${test_dir}")
        return()
    endif()

    set(test_it 0)

    # add tests
    while (test_it LESS num_tests)
        math(EXPR test_it "${test_it} + 1")

        set(TEST_COMMAND "${run_script}" ${test_it})

        # add test
        add_test(
            NAME test::${test_target}::${test_it}
            # execute test
            COMMAND ${TEST_COMMAND}
            # working directory - dircetory with executable file
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        )
    endwhile()
endfunction(target_e2e_tests)

# =================================================================================================

target_e2e_tests("${BITONICSORT_RUNNER}" "${E2E_OUTPUT_SCRIPT}" "${E2E_DAT_DIR}")

# =================================================================================================
