
if (BITONICSORT_MODULES)

set(STACKTRACE_LIB stacktrace)

set(STACKTRACE_SRC_DIR ${DEBUG_DIR}/stacktrace)

set(STACKTRACE_SRCS
    ${STACKTRACE_SRC_DIR}/dump.cppm
)

add_library(${STACKTRACE_LIB})

target_sources(${STACKTRACE_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${STACKTRACE_SRCS}
)

target_include_directories(${STACKTRACE_LIB}
  PRIVATE
    ${INC_DIR}
)

target_link_options(${STACKTRACE_LIB}
  PUBLIC
    -g
    -rdynamic
    -dynamic-export
    -fno-omit-frame-pointer
    -DBOOST_STACKTRACE_USE_ADDR2LINE
)

target_link_libraries(${STACKTRACE_LIB}
  PUBLIC
    boost_stacktrace_from_exception
    boost_stacktrace_backtrace
    dl
    backtrace
)

# set_property(TARGET ${STACKTRACE_LIB}
#     PROPERTY COMPILE_OPTIONS
#         -Wunused-command-line-argument
# )

function(add_target_stacktrace_dump_lib target)
    target_link_libraries(${target}
        PRIVATE
            ${STACKTRACE_LIB}
    )

    # set_property(TARGET ${target}
    #     PROPERTY COMPILE_OPTIONS
    #         -Wunused-command-line-argument
    # )
    # target_compile_options(${target}
    #   PUBLIC
    #     -g
    #     -rdynamic
    #     -dynamic-export
    #     -fno-omit-frame-pointer
    #     -DBOOST_STACKTRACE_USE_ADDR2LINE
    # )

    # target_link_libraries(${target}
    #   PUBLIC
    #     boost_stacktrace_from_exception
    #     boost_stacktrace_backtrace
    #     dl
    #     backtrace
    # )
endfunction(add_target_stacktrace_dump_lib)

endif(BITONICSORT_MODULES)