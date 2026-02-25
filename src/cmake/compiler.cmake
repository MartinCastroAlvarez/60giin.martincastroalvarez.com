function(configure_compiler TARGET_NAME)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${TARGET_NAME} PRIVATE "/Wall")
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "CLion")
        # Add compiler flags or options specific to CLion
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    endif()

endfunction()