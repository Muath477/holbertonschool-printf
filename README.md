# holbertonschool-printf

A custom implementation of `printf` for the Holberton/ALX low-level
programming curriculum.

## Task 0 - `_printf`

`_printf` writes formatted output to stdout, the same way the standard
`printf` does, but only supports these conversion specifiers for now:

* `%c` - prints a single character
* `%s` - prints a string
* `%%` - prints a literal percent sign

### Prototype

```c
int _printf(const char *format, ...);
```

### Return

The number of characters printed (not counting the null byte).

### Files

* `main.h` - header file with all prototypes (include guarded)
* `_printf.c` - `_printf` and its helper functions (`print_char`,
  `print_string`, `print_percent`)

### Compiling

```
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 -Wno-format *.c -o _printf_test
```

(compile together with your own `main.c` that includes `main.h`)

### Authors

Ali Abdulkarim Alhussaini, Muath Hassan Almutairi
