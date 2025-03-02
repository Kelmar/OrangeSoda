import foo;

var a : int;

const five : int = 5;

function baz()
{
    //five = doThing(5);
    a = doThing(five);
}

/*
 * Test block comment!
 */

/*
function doThing(ref bar : int, bar : string) : int
{
    return bar * 2;
}
*/

function doThing(baz : int) : int
{
    return baz * 2;
}

// Implicit void return
function bar()
{
    var b : string;
    //var baz : string;

    a = 6 * 8 + 5;

    while (true)
    {
        if (a)
        {
            a = 0;
            //_write(5, "Foo");

            //_close(_open("myfile", "r"));
        }
    }

    //a = doThing(b);
    //b = doThing();
    //b = doThing(5);
    a = doThing(0);
}

function main()
{
    var c : int;
    bar();
    c = 5 + 10;
}
