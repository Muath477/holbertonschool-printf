# Task 1 - Focus Area Selection

**Selected focus area: Output Buffering & Write Optimization**

## Why this one

Once I actually traced through `_printf`, this was the most obvious thing to dig into. There
is no buffering anywhere in this codebase. None. Every single character that gets printed,
whether it's a literal character from the format string or one digit of a number, goes
through its own call to `write(1, &c, 1)`.

Concretely: `_printf("Hello, %d\n", -1234)` does not do one `write()` call, or even a
handful. It does one `write()` per literal character in `"Hello, "` (7 calls), one for the
`-` sign, one per digit of `1234` (4 calls), and one for `\n`. That's 13 separate `write()`
syscalls for a 13-character line, when real `printf` would do it in one (or, worst case,
however many times the internal `BUFSIZ`-sized buffer fills up and needs a flush).

This matters technically because `write()` is a system call - it crosses into kernel space,
which is orders of magnitude more expensive than an in-memory copy. Real libc `printf`
implementations write into a stream buffer (glibc uses a buffer sized around `BUFSIZ`,
typically 4096 bytes, and the buffering mode - unbuffered/line-buffered/fully buffered -
depends on whether stdout is a terminal or redirected) and only actually call `write()`
when the buffer fills up or the stream is flushed. That's a core architectural decision any
printf implementation has to make: how big a buffer, when to flush, and what the memory/
syscall trade-off looks like.

This codebase skips that decision entirely and just always does the maximally expensive
thing. It's not wrong (the output is byte-for-byte correct), but it's the kind of choice
that would fall apart under any real load - piping a large amount of output through this
`_printf` would be measurably, not just theoretically, slower than glibc's `printf`.

I picked this over the other options mostly because it's the one place where I can point at
something concrete and quantify it (X characters = X syscalls), rather than talking in the
abstract about "safety" or "correctness," which for a codebase this small and free of
`malloc` are honestly not where the interesting problems are.
