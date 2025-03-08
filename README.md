#  Orange Soda
A C like language.

Currently working on a bootstrap compiler in C++.  There are several passes
complete with this though has a few things left to add before we can start
moving on to a selfhosted compiler.

# Goals
The main goal is to provide a language that is a hybrid between, C++/C# with 
a few features from Pascal (sets), Swift, and Ruby that can hopefully be used
for writing both low level situations (embedded systems, and operating systems)
but also provide good feature set for doing high level application development,
E.g.: reflection, exceptiones, and other things expected from modern languages.

* C/C++/C# like syntax
* Easy to interface with hardware at a low level (OS/Driver Development)
  * Pointers & References
  * Pointer arithmetic
  * Structure bit layouts
* Easy to interface with C/C++ (Not sure how yet)
* Stack based objects
* Reflection
* Events
* Lambdas & Closures
* Generics (not templates)
* Interfaces
* Strongly typed
* Const correctness like C++ (read only objects checked at compile time)
* Operator overloading
* Async programming (user space threads (i.e. C# tasks))

# Syntax
Current rough syntax example:
```
import submodule;

var baz : int; // Global variable

function doThing()
{
    baz = 2 + 5 * 10;
}

function doOtherThing(bar : int): int
{
    return bar * 2;
}
```

# Bootstrap
The bootstrap compiler is written in C++ and is dependent on the [fmt 11.0.2](https://fmt.dev/11.0/) library.

The bootstrap compiler is implemented as a recursive decent parser that builds
up an abstract syntax tree and runs several passes over the AST to generate the
final code output.

Things left to add to bootstrap:
- [ ] Variable allocation
- [ ] Constant evaluation (const folding)
- [ ] Arrays and user defined structs
- [ ] Possibly will need boolean short circuit evaluation.

The output is a yet to be written virtual machine, though some experimentation
has been done with interfacing to LLVM.

(Note that the LLVM codegen is currently not up todate.)

## Building
Can be built on most platforms using cmake.

```
mkdir build
cmake ..
make
```
