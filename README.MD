# Multi-Dimensional Array Library (multi_array)

multi_array is a C++ header-only library for working with multi-dimensional arrays. It provides a `multi_array` class template that allows you to create and manipulate multi-dimensional arrays with specified dimensions. The library also includes utility functions for working with built-in arrays and multi-dimensional array conversions.

The motivation here is to design a wrapper class similar to std::array but extends to multi-dimensional arrays. We could, of course, use something like a type alias of nested containers, but we would like to use slightly different semantics. Specifically, we want to be able to access elements with a operator() member using tuples.

The goal is to mirror std::array such that we can use the standard library container semantics. Therefore this class behaves like a container and has many of the usual definitions and member functions (iterators, algorithms, etc.).

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
- [Examples](#examples)
- [Contributing](#contributing)
- [License](#license)

## Features

- Define and work with multi-dimensional arrays in C++.
- Access elements and sub-arrays using iterators and indexing.
- Perform common operations on multi-dimensional arrays.
- Utility functions for working with built-in arrays.
- Conversion functions for creating `multi_array` from built-in arrays.
- Header-only library, no external dependencies.

## Getting Started

### Prerequisites

C++ compiler with C++20 support.

### Installation

This library is header-only, so you don't need to install anything. Simply include the `multi_array.h` file in your C++ project to start using it.

```cpp
#include "multi_array.h"
```

## Usage

1. Include the `multi_array.h` header file in your C++ project.
2. Define a `multi_array` object with the desired dimensions and element type.
3. Use the provided member functions and iterators to work with the multi-dimensional array.

```cpp
#include "multi_array.h"

int main() {
    // Define a 2x3x4 multi-dimensional array of integers.
    multi_array<int, 2, 3, 4> my_array;

    // Access and manipulate elements.
    my_array[0][1][2] = 42;

    // or...
    my_array(0,1,2) = 42;

    // Work with "sub-arrays"
    returns multi_array<int, 4>& sub_array = my_array[0][1]; 

    return 0;
}
```


## Examples

Using multi_array is fairly straightforward if the user is familiar with std::array. Here are some examples to illustrate some of the differences.

### Construction
```cpp
// Nested initializer list construction
multi_array<int, 2, 3> my_array = {{ 1, 2, 3 }, { 4, 5, 6 }}; 
```

### Iteration
```cpp
// Construct an uninitialized 2x3x4 array of doubles
multi_array<double, 2, 3, 4> my_array;

// Range-based loop
for(multi_array<3, 4>& sub_array : my_array) {
  
  //...
}
```

### Accessors
```cpp
// Construct a 2x3 array of doubles
multi_array<double, 2, 3> my_array;

// Gets the first "row" of my_array
multi_array<double, 3> row = get<0>(my_array);

// Gets the 2nd element of the first row of my_array
double element = get<0,1>(my_array);

// Similarly with operator() and operator[]
my_array(0,1) = 42;
my_array[0][1] = 42;

```

### Utilities
```cpp
int my_array[2][3][4];

// Get the number of elements in a built-in array
auto size = array_size(my_array); // returns 24

// Create a multi_array with the appropriate dimensions from a built-in array.
multi_array<int, 4, 5> = to_array(my_array);

// Define a built-in array based on a template alias with variadic extents. C_array<int, 2, 3, ...> = int[2][3][]... 
C_array<int, 2, 3, 4> my_array; // my_array[2][3][4]

```

## Contributing

Contributions to this library are welcome! If you find any issues or have ideas for improvements, please open an issue or create a pull request on the [GitHub repository](https://github.com/tristan-bamford/multi_array).

Before contributing, please review the Contributing Guidelines.

## License

This project is licensed under the [MIT License](LICENSE).



