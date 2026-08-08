# holbertonschool-printf

A custom implementation of `printf` for the Holberton/ALX low-level
programming curriculum.

## Task 0 - `_printf`

`_printf` writes formatted output to stdout, the same way the standard
`printf` does, with these basic conversion specifiers:

- `%c` - prints a single character
- `%s` - prints a string
- `%%` - prints a literal percent sign

### Prototype

```c
int _printf(const char *format, ...);
```

### Return

The number of characters printed (not counting the null byte).

### Files

- `main.h` - header file with all prototypes (include guarded)
- `_printf.c` - `_printf` and its helper functions (`print_char`,
  `print_string`, `print_percent`, `print_int`)

## Task 1 - `%d` and `%i`

`_printf` now also supports signed decimal integers:

- `%d` - prints a signed decimal integer
- `%i` - prints a signed decimal integer

No flags, field width, precision, or length modifiers are handled.

### Return

The number of characters printed (not counting the null byte).

### Compiling

```text
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 -Wno-format *.c -o _printf_test
```

Compile together with your own `main.c` that includes `main.h`.

### Authors

Ali Abdulkarim Alhussaini, Muath Hassan Almutairi
