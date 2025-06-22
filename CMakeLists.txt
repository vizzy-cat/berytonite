cmake_minimum_required(VERSION 3.16)
project(krypton C)

# standard (C23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

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
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

install(TARGETS krypton DESTINATION lib)
install(DIRECTORY include/krypton DESTINATION lib)

#enable_testing()
#add_executable(krypton_test test/krypton_test.c)
#target_link_libraries(krypton_test krypton)
#add_test(NAME KryptonTesting COMMAND krypton_test)
