# Task 0 - Codebase Confirmation and Global Understanding

Codebase analyzed: our own implementation in `holbertonschool-printf` (main.h + _printf.c),
built with Ali Alhussaini. I did not use the provided reference implementation since we
finished the integration ourselves.

## Files

- `main.h` - include-guarded header, pulls in `stdarg.h` and `unistd.h`, declares every
  function used across the project.
- `_printf.c` - everything lives in one file: `_printf` itself plus 10 helper functions.

Note: the official Holberton spec usually wants one function per file (Betty style, strict
`.h`/`.c` split per prototype). We didn't do that here, we just kept every handler in
`_printf.c`. That's a real constraint inherited from how we built the project, not a
deliberate design choice, and it's worth flagging before going further.

## Architecture

`_printf` is a single pass over the format string, character by character:

1. If the current char isn't `%`, write it straight to stdout and move on.
2. If it's `%`, look at the next char and dispatch to the matching handler through an
   if/else chain (not a table/array of function pointers, just a long if/else).
3. If `%` is the last char in the string (dangling `%`), bail out and return -1.
4. If the char after `%` doesn't match any known specifier, print `%` followed by that
   char literally (so `_printf("%z")` prints `%z`).

Supported specifiers: `%c %s %% %d %i %u %o %x %X %b`. No flags, no width, no precision,
no length modifiers. `%b` (binary) isn't a real printf specifier, it was a bonus task.

## Major components and responsibilities

- `_printf` - owns the loop, the `%` detection, and the dispatch. Also owns `va_start`/
  `va_end` and the running `count`.
- `print_char`, `print_string`, `print_percent` - the base task handlers, each takes the
  `va_list` and does one `write()`.
- `print_int(long n)` - recursive signed decimal printer. Note it takes `long`, not
  `va_list` - `_printf` calls `va_arg(args, int)` itself and passes the result in. It's the
  odd one out signature-wise compared to every other handler.
- `print_base(unsigned int n, unsigned int base, char *digits)` - shared recursive base
  converter, used by `print_unsigned`, `print_octal`, `print_hex`, `print_hex_upper`. Each
  of those is basically a one-line wrapper that just picks a base and a digit alphabet.
- `print_binary` - does NOT go through `print_base` even though base 2 would work fine
  there. It has its own iterative bit-shifting loop instead. Inconsistent with the pattern
  above, more on that later.

## Constraints inherited from the original project

- Curriculum rules for Holberton/ALX low-level projects forbid using `printf`, `puts`, or
  any other stdio function that outputs - only `write()` is allowed. That's why every
  single handler bottoms out in `write()` calls instead of buffering internally like real
  libc printf does.
- Compiled with `-Wall -Wextra -Werror -pedantic -std=gnu89`, sometimes with
  `-Wno-format`. `gnu89` means old-style C: no mixed declarations and code, no `//`
  comments in the strict checker (we use them anyway informally but the graded build
  target is C89-ish).
- Return value contract has to match real `printf`: number of characters printed, or -1 on
  error (NULL format, or a dangling `%`).
- No `malloc`/`free` anywhere in the codebase. Everything is written directly, byte by
  byte or string by string, with no intermediate buffer.

That last point turned out to matter a lot once I picked my focus area - see the next file.
