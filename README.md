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

## Task 2 - `%u`, `%o`, `%x`, `%X`

`_printf` now also supports unsigned integer conversions:

- `%u` - prints an unsigned decimal integer
- `%o` - prints an unsigned integer in octal
- `%x` - prints an unsigned integer in lowercase hexadecimal
- `%X` - prints an unsigned integer in uppercase hexadecimal

### Return

The number of characters printed (not counting the null byte).

## Task 3 - `%b` (bonus)

`_printf` also supports a non-standard specifier:

- `%b` - prints an unsigned integer in binary, with no leading zeros

### Return

The number of characters printed (not counting the null byte).

## Examples

```c
#include "main.h"

int main(void)
{
	_printf("Char: %c\n", 'H');
	_printf("String: %s\n", "Hello");
	_printf("Percent: %%\n");
	_printf("Int: %d, %i\n", -42, 42);
	_printf("Unsigned: %u\n", 42);
	_printf("Octal: %o\n", 42);
	_printf("Hex: %x, %X\n", 255, 255);
	_printf("Binary: %b\n", 5);
	return (0);
}
```

Output:

```text
Char: H
String: Hello
Percent: %
Int: -42, 42
Unsigned: 42
Octal: 52
Hex: ff, FF
Binary: 101
```

## Testing notes

Output was compared manually against real `printf` on the same inputs
(WSL/Linux, glibc), including edge cases: a format string ending in a
lone `%`, `%s` with a `NULL` argument, and negative numbers with `%d`.

### Authors

Ali Abdulkarim Alhussaini, Muath Hassan Almutairi
