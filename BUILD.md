## Requirements

- CMake ≥ 3.16

- C Compiler with C17 support (don't use MSVC, it's still buggy)

- Make / Ninja

- CTest (optional)

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

- `libberyton.so` / `libberyton.dll` / `libberyton.dylib` (shared/dynamic library)

- Test binaries

## Running Tests

```
cd build
ctest --output-on-failure
```

---

## Install
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
