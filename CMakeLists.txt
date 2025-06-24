cmake_minimum_required(VERSION 3.16)
project(krypton C)
include(CTest)

# standard (C23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# compiler flags
add_compile_options(-O2 -Wall -Wpedantic -Werror -Wextra)

# source code
file(GLOB_RECURSE KRYPTON_SRC CONFIGURE_DEPENDS
	src/*.c
)

# header
file(GLOB_RECURSE KRYPTON_HEADER CONFIGURE_DEPENDS
	include/krypton/*.h
)

add_library(krypton STATIC ${KRYPTON_SRC} ${KRYPTON_HEADER})

target_include_directories(krypton PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/krypton>
	$<INSTALL_INTERFACE:include>
)

# install
install(TARGETS krypton DESTINATION lib)
install(DIRECTORY include/krypton DESTINATION lib)

# testing
enable_testing()

set(TEST_FILES
	test/base64.c
)

foreach(test_src ${TEST_FILES})
	get_filename_component(test_name ${test_src} NAME_WE)
	add_executable(${test_name} ${test_src})
	target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_link_libraries(${test_name} PRIVATE krypton)
	add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
