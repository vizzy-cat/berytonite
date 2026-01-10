cmake_minimum_required(VERSION 3.16)
project(berytonite LANGUAGES C)

# C standard: C17 without extensions
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Default to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Detect if the compiler or the target is support some features
include(CheckSymbolExists)
check_symbol_exists(explicit_bzero "string.h" HAVE_EXPLICIT_BZERO)

# Compiler flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		message(STATUS "Build mode: DEBUG")
		add_compile_options(-Og -g -msse2 -DNOPTIMIZE -DENABLE_SSE)
else()
	message(STATUS "Build mode: RELEASE")
	add_compile_options(-msse2 -DENABLE_SSE)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	add_compile_options(
		-Wduplicated-cond
		-Wduplicated-branches
		-Wshadow
		-Wsign-conversion
		-Wlogical-op
		-Wjump-misses-init
		-Wdouble-promotion
		-Wformat=2
		-Wconversion
		-fanalyzer
		-Walloc-zero
		-Walloca
	)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	add_compile_options(
		-Wnull-dereference
		-Wconversion
		-Wfloat-equal
		-Wdouble-promotion
		-Wswitch-enum
		-Wunreachable-code
		-Wmissing-noreturn
		-Wmissing-prototypes
		-Wstring-conversion
		-Wimplicit-fallthrough
		-fanalyzer
		-Wcomma
		-Wextra-semi
		-Wshadow-all
	)
endif()

# Source files
file(GLOB_RECURSE BERYTON_SRC CONFIGURE_DEPENDS src/*.c)
file(GLOB_RECURSE BERYTON_HEADERS CONFIGURE_DEPENDS include/bt/*.h)

# Shared Library
add_library(beryton_shared SHARED ${BERYTON_SRC} ${BERYTON_HEADERS})
set_target_properties(beryton_shared PROPERTIES OUTPUT_NAME beryton)
target_include_directories(beryton_shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/bt>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Static Library
add_library(beryton_static STATIC ${BERYTON_SRC} ${BERYTON_HEADERS})
set_target_properties(beryton_static PROPERTIES OUTPUT_NAME beryton)
target_include_directories(beryton_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/bt>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# macOS specific
if(APPLE)
	target_link_libraries(beryton_shared PUBLIC "-framework Security")
endif()

# Windows specific
if(WIN32)
	target_link_libraries(beryton_static PUBLIC bcrypt)
	target_link_libraries(beryton_shared PUBLIC bcrypt)
endif()

# Install rule
install(TARGETS beryton_static beryton_shared
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/bt DESTINATION include)

# Testing
include(CTest)
enable_testing()

set(TEST_FILES
    test/base64.c
    test/sha256.c
)

foreach(test_src ${TEST_FILES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})

    target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${test_name} PRIVATE beryton_static) # Test against static lib

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
