cmake_minimum_required(VERSION 3.16)
project(berytonite LANGUAGES C)

# C standard: C23 without extensions
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Use Release by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler flags by build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build mode: DEBUG + TESTING")
    add_compile_definitions(DEBUG TESTING)
    add_compile_options(-g)
else()
    message(STATUS "Build mode: RELEASE")
    add_compile_definitions(NDEBUG)
    add_compile_options(-O2)
endif()

# Common warning flags
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Source and header files
file(GLOB_RECURSE BERYTON_SRC CONFIGURE_DEPENDS src/*.c)
file(GLOB_RECURSE BERYTON_HEADERS CONFIGURE_DEPENDS include/beryton/*.h)

# Library target
add_library(beryton STATIC ${BERYTON_SRC} ${BERYTON_HEADERS})

# Public include path
target_include_directories(beryton PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/beryton>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/beryton>
)

# Install rules
install(TARGETS beryton DESTINATION lib)
install(DIRECTORY include/beryton DESTINATION include)

# Testing
include(CTest)
enable_testing()

# List of test files
set(TEST_FILES
    test/base64.c
    test/sha256.c
)

# Setup test executables
foreach(test_src ${TEST_FILES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})

    target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${test_name} PRIVATE beryton)

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
