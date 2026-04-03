Assignments in OrangeSoda are statements like in Pascal.

This means that the following is NOT leagal in OrangeSoda like it would be in other C based languages:

```osoda
var foo : int;

if ((foo = 5) == 10) { } // Syntax error
```

You instead must write the following:
```osoda
var foo : int;

foo = 5;
if (foo == 10) { } // Okay, no assigment inside the if.
```

The trade off is that you also cannot write:
```osoda
var foo, bar : int;

foo = bar = 5; // Illegal, assignment of bar is not an expression.
```
