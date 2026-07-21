I would like to have a range type allowing for Pascal style use of `in` keyword to be used for testing for items in any sort of collection of elements.

```osoda
function isHexChar(c: char): bool
{
    return c in ['0'..'9', 'A'..'F', 'a'..'f'];
}
```
