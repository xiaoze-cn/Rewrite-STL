# Data Structures for Beginners

Data Structures for Beginners is an educational C++23 data structures and algorithms package. It keeps the implementations readable while using modern C++ facilities such as `std::expected` for error handling.

## Features

- Header-only package layout.
- C++23 `ds::ds` interface target for CMake users.
- Headers grouped by textbook domain: linear, tree, hash, set, graph, algorithms, and common helpers.
- Error handling based on `std::expected`.

## Build

Pixi manages the compiler and all build tools. Run the configure and build workflow with:

```powershell
pixi run check
```

## Layout

```text
Data Structures for Beginners/
├── .pixi/
│   └── config.toml
├── include/
│   └── ds/
│       ├── algorithm.hpp
│       ├── algorithm/
│       │   ├── match.hpp
│       │   ├── search.hpp
│       │   ├── select.hpp
│       │   └── sort.hpp
│       ├── common.hpp
│       ├── common/
│       │   ├── error.hpp
│       │   └── random.hpp
│       ├── graph.hpp
│       ├── graph/
│       │   ├── list.hpp
│       │   └── matrix.hpp
│       ├── hash.hpp
│       ├── hash/
│       │   ├── chain.hpp
│       │   └── table.hpp
│       ├── linear.hpp
│       ├── linear/
│       │   ├── list.hpp
│       │   ├── queue.hpp
│       │   └── stack.hpp
│       ├── set.hpp
│       ├── set/
│       │   └── unionfind.hpp
│       ├── tree.hpp
│       ├── tree/
│       │   ├── binary.hpp
│       │   └── heap.hpp
│       └── ds.hpp
├── .gitignore
├── CMakeLists.txt
├── pixi.lock
├── pixi.toml
└── README.md
```
