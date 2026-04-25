# Xenon Lang

Xenon Lang is an experimental scripting language built on top of a modified Lua runtime.

It keeps the lightweight and embeddable nature of Lua, while introducing a cleaner C-style syntax, custom module loading, and an extended runtime library.

---

# Features

- C-style blocks with braces
- `if / elseif / else`
- `while`
- `do { } while(condition);`
- Numeric `for`
- `let` variable declarations
- `continue`
- `!=` operator
- `+` string concatenation
- `params` varargs syntax
- Custom `include`
- Custom `import`
- Built-in `console` library
- Built-in `file` library
- Custom advanced string type

---

# Example

```xe
function main(params args) {

    if (#args > 1) {
        console.writeln("More then 1");
    }

    for (i = 1; #args) {
        console.writeln(args[i]);
    }

    let idx = 0;

    do {
        idx = idx + 1;
        console.writeln(idx);
    }
    while (idx < 10);
}

main(1, 2, 3);
```

---

# Syntax

## Variables

```xe
let name = "Xenon";
let count = 10;
```

## If / Else

```xe
if (count > 10) {
    console.writeln("large");
}
elseif (count == 10) {
    console.writeln("equal");
}
else {
    console.writeln("small");
}
```

## While

```xe
let i = 0;

while (i < 5) {
    i = i + 1;
}
```

## Do While

```xe
do {
    console.writeln("loop");
}
while(false);
```

## For

```xe
for(i = 1; 10) {
    console.writeln(i);
}
```

## Functions

```xe
function greet(name) {
    console.writeln("Hello " + name);
}
```

## Varargs

```xe
function main(params args) {
    console.writeln(#args);
}
```

---

# Differences From Lua

| Lua | Xenon |
|---|---|
| `then/end` | `{}` |
| `repeat/until` | `do {} while()` |
| `~=` | `!=` |
| `..` | `+` |
| `...` | `params name` |
| `local` | `let` |

---

# Modules

## Include

Execute another file for side effects:

```xe
include "libs/io.xe"
```

## Import

Load a module and receive a result:

```xe
let math = import("math.xe");
```

Recommended module style:

```xe
return {
    add = function(a, b) {
        return a + b;
    }
};
```

---

# Runtime Libraries

# Console

## console.write(...)

Writes values without newline.

```xe
console.write("Hello");
```

## console.writeln(...)

Writes values with newline.

```xe
console.writeln("Hello World");
```

## console.readln()

Reads one line from stdin.

```xe
let input = console.readln();
```

---

# File

## file.get_path(path)

Returns absolute path.

```xe
let path = file.get_path("data.txt");
```

## file.get_dir(path)

Returns directory path.

```xe
let dir = file.get_dir(path);
```

## file.get_home_path()

Returns configured home path.

## file.set_home_path(path)

Sets home path.

---

# Strings

Xenon uses a custom string implementation with methods and operators.

## Concatenation

```xe
let text = "Hello " + "World";
```

## Length

```xe
#text
```

## Indexing

```xe
text[1]
```

## Methods

```xe
text.to_upper()
text.to_lower()
text.index_of("abc")
text.last_index_of("abc")
text.substring(2, 3)
text.contains("abc")
text.starts_with("He")
text.ends_with("ld")
text.replace("a", "b")
text.trim()
text.reverse()
text.rep(3)
text.split(",")
text.is_empty()
```

## Join

```xe
string.join(",", {"a", "b", "c"})
```

---

# Build

```sh
./build.sh
```

Typical manual build:

```sh
mkdir build
cd build
cmake ../src
cmake --build .
```

---

# Run

```sh
./xenon -F script.xe
```

Example:

```sh
./xenon -F examples/basic.xe
```

---

# Project Goals

Xenon is designed as an experimental language project focused on:

- language design
- parser/compiler customization
- Lua VM extension
- embeddable scripting
- cleaner syntax
- runtime experimentation

---

# Status

Experimental / Work in Progress.

---
