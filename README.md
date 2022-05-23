# msglang: A messsage passing language implemented in pure C++

msglang is a special-purpose, message-passing language developed as a project for my [Software Engineering](https://www.csd.uoc.gr/~hy352/) course.
It uses "Message Object" variables which take the role of a "Sending Object" or a "Receiving Object".
The language is implemented in, and compiled to, pure C++, meaning that Overloading and the Pre-Processor were hacked and bend to their knees.


## Keywords
- ``let <id>``: Declares an Object with ``id`` name. Always followed by an initializer.
- ``values <v1>, <v2>, ...``: Groups every comma delimited argument to it's right into an aray, indexed using ascending numeric strings, i.e ```"0", "1",...```.
- ``key(<id>)``: Declares an Object member variable, which we can later access by using the ``id`` symbol.
- ``func(id) { <body> }``: Declares a member function with a specific name (``id``) and body (``body``).
- ``lambda { <body> }``: Declares an anonymous member function. Must be saved to a member variable.
- ``self(<id>)``: Returns the **Receiver's** member variable identified by ``id``. 
- ``arg(<id>)``: Returns the **Sender's** member variable identified by ``id``.
- ``call(<id>)`` Used by the **Sender**. Specifies a function of the **Receiver** to call.
- ``return <val>``: Return the specified value
- ``none``: The language's empty value.

## Built-in functions
- ``input(<msg>)``: Prints ``msg`` to ``stdout`` and reads ``stdin``. Automatically coverts user input to one of ``double``, ``int``, ``bool`` or ``string``

## Building
```
cd msglang
make
```
This will produce a ``demo`` binary, which demonstrates some of the language's functionality.

## Cleaning
```
make clean
```
