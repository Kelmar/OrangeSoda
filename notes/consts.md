Should be noted that the `const` and `readonly` keywords in OrangeSoda
will behave more like C#'s versions.


# `const`
In this sense `const` describes a compile time value; and does not reserve
any space in the runtime object that it resides in.

# `readonly`
Similar to C#, but behaves a bit more like C++'s version of `const` in
that it reserves space inside the runtime object for that variable, and
can be initialized at runtime.

Like C++ this would be allowable to use to indicate a contract that a
function will not modify an object that gets passed in.

```osoda
class MyPrinter
{
    // Function stipluates it will not modify w.
    public void Print(readonly Widget w)
    {
        
    }
}
```
