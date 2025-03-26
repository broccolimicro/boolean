# Boolean

A C++ library implementing Boolean algebra operations using cubes and covers for efficient representation and manipulation of Boolean functions. This library provides the necessary primitives for digital logic synthesis, verification, and optimization.

## Overview

The Boolean library represents Boolean expressions using a four-valued logic system, providing powerful abstractions for working with Boolean functions. It's particularly useful in:

- Digital circuit design and synthesis
- Logic minimization and optimization
- Formal verification
- Boolean function analysis and transformation
- Arithmetic circuit synthesis

## Core Components

### Cubes

A cube represents a product term in Boolean algebra (conjunction of literals). In the Boolean library, it uses a four-valued logic system:

- `-` (11): Don't care, matches both 0 and 1
- `1` (10): Positive literal, matches only 1
- `0` (01): Negative literal, matches only 0
- `X` (00): Empty or null, matches nothing

For example, the expression `x & ~y & z` would be represented as a cube with x=1, y=0, z=1.

```cpp
#include <boolean/cube.h>
using namespace boolean;

// Create cubes
cube a(0, 1);  // Variable 0 is true (positive)
cube b(1, 0);  // Variable 1 is false (negative)
cube c = a & b;  // Logical AND of cubes

std::cout << c << std::endl;
```

### Covers

A cover represents a sum-of-products expression - a collection of cubes combined with OR operations. Covers can express any Boolean function and are the foundation for logic minimization algorithms.

```cpp
#include <boolean/cover.h>
using namespace boolean;

// Create cubes
cube a(0, 1);  // x
cube b(1, 0);  // ~y
cube c(2, 1);  // z

// Create covers
cover f1 = a & b;  // x & ~y
cover f2 = b & c;  // ~y & z
cover result = f1 | f2;  // x & ~y | ~y & z

std::cout << result << std::endl;
```

## Logic Minimization

The library implements the Espresso algorithm for logic minimization, finding a minimal representation of a Boolean function through:

1. Expansion of cubes
2. Identification of essential prime implicants
3. Reduction of redundant terms
4. Creation of an irredundant cover

```cpp
#include <boolean/cover.h>
using namespace boolean;

// Create the function to minimize
cover F;
F.push_back(cube(0, 1) & cube(1, 1));  // x & y
F.push_back(cube(0, 1) & cube(1, 0));  // x & ~y
F.push_back(cube(0, 0) & cube(1, 1));  // ~x & y

// Minimize using Espresso
cover D;  // Don't care set (empty)
cover R = ~F;  // Off-set (complement of F)
espresso(F, D, R);

std::cout << "Minimized function: " << F << std::endl;
```

## Advanced Components

Building on the core concepts, the Boolean library provides several advanced components for more complex operations.

### Bitset

The bitset class represents an ordered collection of Boolean functions, serving as a foundation for multi-bit operations. It stores a vector of covers, where each cover represents a single bit in the collection. Bitset provides standard logical operations (AND, OR, XOR, NOT), shift operations, and bit manipulation functions. It also includes advanced capabilities such as Espresso-based minimization, cofactoring for logic specialization, and sophisticated algorithms for partitioning complex functions and decomposing them into simpler representations. This component is specifically designed to facilitate logic synthesis of arithmetic operators and complex multi-bit operations, making it ideal for generating optimized circuit implementations.

```cpp
#include <boolean/bitset.h>
using namespace boolean;

// Create a bitset with 4 bits (variables 0-3)
bitset b(4, 0);

// Perform logical operations
bitset result = b & (b << 1);  // AND with shifted version
result.espresso();  // Minimize each bit function

// Extract a subset of bits
bitset subset = result.subset(0, 2);  // Get bits 0 and 1
```

### Arithmetic Operations

Built on top of the bitset abstraction, the library provides symbolic representations for arithmetic operations:

#### Unsigned Integer

The `unsigned_int` class extends bitset to represent unsigned integer values symbolically. Designed specifically for logic synthesis of arithmetic circuits, it implements full arithmetic operations (addition, subtraction, multiplication, division) and comparison operators, all working on symbolic Boolean expressions rather than concrete values. The implementation handles carry propagation, overflow, and other arithmetic concerns in ways that directly translate to hardware implementations. It provides decomposition algorithms for identifying common subexpressions and factoring complex logic, enabling efficient circuit-level optimizations for digital arithmetic units and datapaths.

```cpp
#include <boolean/unsigned_int.h>
using namespace boolean;

// Create symbolic unsigned integers
unsigned_int a(4, 0);  // 4-bit value using variables 0-3
unsigned_int b(8);     // 8-bit constant value

// Perform arithmetic
unsigned_int sum = a + b;
unsigned_int product = a * b;

// Compare values
cover comparison = (a < b);  // Returns a cover representing when a < b
```

#### Signed Integer

The `signed_int` class extends bitset for signed integer representation using two's complement. Tailored for synthesizing signed arithmetic circuits, it provides proper sign extension functionality, ensuring correct handling of the sign bit during operations. The class implements complete arithmetic operations with appropriate sign handling for negation, addition, subtraction, multiplication, and division, allowing for direct translation to hardware implementations. Comparison operators are also sign-aware, correctly handling the special cases of negative vs. positive number comparison. This makes it particularly valuable for modeling and synthesizing optimized signed arithmetic circuits in hardware design.

```cpp
#include <boolean/signed_int.h>
using namespace boolean;

// Create symbolic signed integers
signed_int a(4, 0);   // 4-bit signed value using variables 0-3
signed_int b(-5);     // Constant with value -5

// Perform arithmetic with sign handling
signed_int difference = a - b;
signed_int negated = -a;     // Two's complement negation

// Sign-aware comparison
cover is_negative = (a < signed_int(0));  // Check if a is negative
```

### Variable Mapping

The mapping class provides translation between different variable identifier spaces, which is essential when combining Boolean expressions from different contexts. It acts as a variable renaming mechanism, with bidirectional map and unmap operations for translating variable IDs. The class supports identity mapping (no translation), composition with other mappings, and reverse mapping generation. This functionality is particularly useful in hierarchical designs where different modules may have locally-defined variables that need consistent translation when integrated into a larger system.

```cpp
#include <boolean/mapping.h>
using namespace boolean;

// Create a mapping
mapping m;
m.set(0, 5);  // Map variable 0 to variable 5
m.set(1, 3);  // Map variable 1 to variable 3

// Apply mapping to a cube
cube c(0, 1);  // Variable 0 is true
c.apply(m);    // Now variable 5 is true

// Create reverse mapping
mapping reverse = m.reverse();  // Maps 5->0, 3->1
```

## Building and Installation

The library can be built using the provided Makefile:

```bash
make
```

This will create `libboolean.a` which can be linked with your application.

### Dependencies

This library has no dependencies other than the C++ Standard Template Library (STL).

## License

Licensed by Cornell University under GNU GPL v3.

Written by Ned Bingham.
Copyright © 2020 Cornell University.

Haystack is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Haystack is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License may be found in COPYRIGHT.
Otherwise, see <https://www.gnu.org/licenses/>.

