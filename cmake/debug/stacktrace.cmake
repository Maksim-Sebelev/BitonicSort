if (CMAKE_CXX_STANDARD GREATER_EQUAL 20)

  set(STACKTRACE_LIB stacktrace)

  set(STACKTRACE_SRC_DIR ${DEBUG_DIR}/stacktrace)

  set(STACKTRACE_SRCS
      ${STACKTRACE_SRC_DIR}/dump.cppm
  )

  set(STACKTRACE_SHOW_MACRO SHOW_EXCEPTION_STACKTRACE)

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
      -rdynamic
      -dynamic-export
      # -fno-omit-frame-pointer
  )

  target_compile_options(${STACKTRACE_LIB}
    PUBLIC
      -g
      -fno-omit-frame-pointer
  )
  target_compile_definitions(${STACKTRACE_LIB}
    PUBLIC
      ${STACKTRACE_SHOW_MACRO}
      BOOST_STACKTRACE_USE_ADDR2LINE
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
  endfunction(add_target_stacktrace_dump_lib)


  function(add_target_stacktrace_dump_lib_on_debug target)
      target_link_libraries(${target}
        PRIVATE
          $<$<CONFIG:Debug>:${STACKTRACE_LIB}>
      )
  endfunction(add_target_stacktrace_dump_lib_on_debug)


else()

  message(WARNING "This library required c++20 for C++ modules. Functions from here will made nothing.")

  function(add_target_stacktrace_dump_lib target)
      message(WARNING )
  endfunction(add_target_stacktrace_dump_lib)


  function(add_target_stacktrace_dump_lib_on_debug target)
      target_link_libraries(${target}
        PRIVATE
          $<$<CONFIG:Debug>:${STACKTRACE_LIB}>
      )
  endfunction(add_target_stacktrace_dump_lib_on_debug)

endif(CMAKE_CXX_STANDARD GREATER_EQUAL 20)

