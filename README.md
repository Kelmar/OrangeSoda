#  Orange Soda
A C like language.

# Syntax
```
import submodule;

var baz : int; // Global variable

function doThing()
{
    baz = 2 + 5 * 10;
}

function doOtherThing(bar : int) : int
{
    return bar * 2;
}
```

# Bootstrap
The bootstrap compiler is written in C++ and is dependent on the [fmt 11.0.2](https://fmt.dev/11.0/) library.

Thing's to add to bootstrap:
- [ ] Basic return analysis (make sure 'return' is used for functions that need it.)
- [ ] Variable allocation
- [ ] Constant evaluation

## Building
Can be built on most platforms using cmake.

```
mkdir build
cmake ..
make
```