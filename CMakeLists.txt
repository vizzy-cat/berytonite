cmake_minimum_required(VERSION 3.16)
project(berytonite C)
include(CTest)

# standard (C23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# compiler flags
add_compile_options(-O2 -Wall -Wpedantic -Werror -Wextra)

# source code
file(GLOB_RECURSE BERYTON_SRC CONFIGURE_DEPENDS
	src/*.c
)

# header
file(GLOB_RECURSE BERYTON_HEADER CONFIGURE_DEPENDS
	include/beryton/*.h
)

add_library(beryton STATIC ${BERYTON_SRC} ${BERYTON_HEADER})

target_include_directories(beryton PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/beryton>
	$<INSTALL_INTERFACE:include/beryton>
)

# install
install(TARGETS beryton DESTINATION lib)
install(DIRECTORY include/beryton DESTINATION lib)

# testing
enable_testing()

set(TEST_FILES
	test/base64.c
)

foreach(test_src ${TEST_FILES})
	get_filename_component(test_name ${test_src} NAME_WE)
	add_executable(${test_name} ${test_src})
	target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_link_libraries(${test_name} PRIVATE beryton)
	add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
