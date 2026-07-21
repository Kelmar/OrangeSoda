# C++ issues
Building out a test OS in C++ I realized that the language likes to add all
sorts of extra things to structs if you aren't careful.  For example

```cpp
struct PageDirectory
{
    const int size = 0x1000; // This generates a member variable.

    /*
     * You can't do this because size is a variable, not a compile
     * time constant.
     */
    PageEntry entries[size]; 
};
```

Things get more sticky if you try to use `constexpr` in the struct, as that
still has odd side effects on the struct's generation:

```cpp
struct PageDirectory
{
    static constexpr int size = 0x1000;

    // Okay, but can get member alignment warnings if we're not careful?
    PageEntry entries[size]; 
}
```

This seems odd, for OrangeSoda I think it would be best for structs and unions
to behave like they do in C; they should just be basic containers for items.

Meaning, they should never carry a vtable or add hidden member variables.  This
does mean that unions and structs cannot take place in any sort of inheritance,
but I think this is a reasonable trade off for making them friendly to working
with low level hardware interfacing.

Any "member" functions would actually end up getting implemented as a set of
extension type methods.  So for example:

```osoda
struct PageDirectory
{
    // Actually declares a compile time constant, not a variable.
    const int size = 0x1000;

    function Update()
    {
        // Do thing here.        
    }
}
```

Effectively become syntactic sugar for:
```osoda
struct PageDirectory
{
    // Actually declares a compile time constant, not a variable.
    const int size = 0x1000;
}

extend PageDirectory
{
    function Update()
    {
        // Do thing here.
    }
}
```

# Additional Thoughts
`struct` and `union` types should never have non-public members.  Extensions
may have their own non-public methods and properties.

`enum` is like C++ class enum, must be qualified with enum name.  Also considering Pascal style `set`, a.k.a. flags; but allowing for us of `in` keyword for flag tests:

```osoda
set WindowFlags
{
    MinimizeBox,
    MaximizeBox,
    CloseBox,
    SystemMenu,
    Sizeable
}

function FlagTest()
{
    var flags : WindowFlags;
    flags = [ WindowFlags::MinimizeBox, WindowFlags::MaximizeBox ];
    flags += WindowFlags::Sizable; // Add to set, same as bitwise | in C

    if (WindowFlags::Sizable in flags) // For C: if ((flags & FOO) == FOO)
    {
        // Do Sizable is set
    }

    flags -= WindowFlags::Sizable; // Remove from set, same as bitwise & ~(FLAG) in C
}
```
