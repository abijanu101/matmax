# matmax
a simple DSL for matrix manipulation, our Compiler Construction course project for the semester of Fall '26.


| Contributor | ID |
| :--- | :--- |
| Ch. Abdul Rehman | 23L-0577 |
| Areeba Naeem | 23L-9656 |
| Eman Naeem | 23F-0808 |


## Description
Conventional matrix implementations restrict operations to homogeneous integers or floating-point numbers. Real-world data processing (tabular data analysis, symbolic algebra, graph theory, string manipulation) frequently relies on 2D structures containing varied data types such as characters, strings, polynomials, or custom structures.

Our Domain-Specific Language (DSL) generalizes matrix-based computation beyond standard numeric types. It provides native syntax for declaring matrix algebraic properties, type resolution rules, and custom operation semantics. Operator overloading, type inference rules, and dynamic fallback logic live directly in the type definition, which removes complex 2D iteration and manual type conversion code. Built-in boundary and structural validation reduces runtime errors.

## Dependencies:
* Flex
* Bison
* Make
* The G++ Compiler
