## Requirements

- CMake ≥ 3.16

- C Compiler with C17 support (e.g., GCC ≥ 9, Clang ≥ 10)

- Make as build system

- CTest for running tests (optional)

## Building the Library

**1. Clone the repo**
```
git clone https://github.com/vizzy-cat/berytonite.git
cd berytonite
```
**2. Configure (Debug or Release)**
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
```
To build with debugging :
```
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```
> Note : default mode is Release if not specified.

**3. Build**
```
cmake --build build --parallel
```
This will build:

- `libberyton.a` (static library)

- `libberyton.so` or `libberyton.dll` (shared/dynamic library)

- Test binaries

## Running Tests

```
cd build
ctest --output-on-failure
```

---

## Install (Optional)
```
cmake --install build --prefix /your/install/path
```
Installs to:

- `lib/` → compiled library

- `include/beryton/` → public headers



---

## Link in Your Project

In your CMake project:
```
find_library(BERYTON_LIB beryton PATHS /your/install/path/lib)
target_link_libraries(your_target PRIVATE ${BERYTON_LIB})
target_include_directories(your_target PRIVATE /your/install/path/include)
```
