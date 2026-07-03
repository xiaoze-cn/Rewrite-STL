# Containerium

Containerium is an educational C++23 data-structure and algorithm package. It keeps the implementations readable while using modern C++ facilities such as `std::expected` for error handling.

## Features

- Header-only package layout.
- C++23 interface target for CMake users.
- Data structures grouped by domain: containers, structures, algorithms, utilities, and core helpers.
- Error handling based on `std::expected`.

## Layout

```text
Containerium
├── CMakeLists.txt
├── README.md
└── include
    └── containerium
        ├── containerium.hpp
        ├── algorithms
        │   ├── select.hpp
        │   ├── sort.hpp
        │   └── string.hpp
        ├── containers
        │   ├── hash.hpp
        │   ├── heap.hpp
        │   ├── linear.hpp
        │   ├── queue.hpp
        │   └── stack.hpp
        ├── core
        │   └── error.hpp
        ├── structures
        │   ├── graph.hpp
        │   ├── tree.hpp
        │   └── unionfind.hpp
        └── utilities
            └── random.hpp
```
