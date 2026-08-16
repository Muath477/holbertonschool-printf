# What happens when you point an AI code reviewer at your printf clone

## Introduction

For the Holberton/ALX low-level programming track, my teammate Ali and I built our own
version of `printf` in C - a two-file implementation (`main.h` + `_printf.c`) that supports
`%c`, `%s`, `%%`, `%d`/`%i`, `%u`, `%o`, `%x`/`%X`, and a bonus `%b` for binary. No flags,
no width, no precision, no length modifiers, only `write()` allowed for output (the
curriculum forbids `printf`/`puts` and anything else in stdio that outputs text).

For this project, the assignment was to hand that codebase to an AI tool, ask it to review
the code as a reviewer (not a co-author), and then critique the review myself instead of
just accepting it. The point isn't to get better code out of the AI - it's to find out where
its reasoning about low-level C actually holds up and where it doesn't.

## Focus Area

I picked **Output Buffering & Write Optimization**. Once I traced through `_printf`, it was
obvious this was where the real story was: there's zero buffering anywhere. Every character
that gets printed - a literal char from the format string, a single digit, whatever -
triggers its own `write(1, &c, 1)` syscall. Printing something like `"Hello, %d\n"` with a
4-digit number costs around a dozen separate syscalls for a 13-character line, where real
libc `printf` would batch that into one or two writes against an internal buffer (typically
sized around `BUFSIZ`). It's the one part of this codebase where I could point at something
concrete and quantify the cost, instead of talking abstractly about "safety," which for a
project with no `malloc` anywhere isn't really where the interesting problems live.

## AI Prompt

Exact prompt I used, verbatim:

```
I'm going to give you the complete source of a small C implementation of printf,
written for a low-level programming school project (Holberton/ALX curriculum,
compiled with -Wall -Wextra -Werror -pedantic -std=gnu89, only write() allowed
for output, no printf/puts/stdio output functions permitted).

Act as a code reviewer, not an author. Do NOT rewrite the code, do NOT propose an
optimized version, and do NOT fix anything yourself - just review what's there.

Give me feedback specifically on:
1. Code structure and readability
2. Logical correctness
3. Memory safety and low-level concerns (pointers, va_list usage, buffer handling)
4. Edge cases and undefined behavior
5. Efficiency and architectural decisions

Here is main.h:
[contents of main.h]

Here is _printf.c:
[contents of _printf.c]
```

## Summary of AI Feedback

The review came back in five sections matching the prompt. On structure, it flagged the
if/else dispatch chain as something that could be a lookup table instead, called out that
`print_binary` duplicates logic that `print_base` already provides, and noted `print_int`
has an inconsistent signature (`long` instead of `va_list`) compared to every other handler.
On correctness, it traced the index increments in the main loop and didn't find an
off-by-one, and gave credit for the NULL-guard in `print_string` and the dangling-`%`
handling. On memory safety, it noted there's no dynamic allocation, then raised two specific
concerns: passing `va_list` by value into helper functions as a potential source of
undefined behavior, and the recursive digit-printing in `print_int`/`print_base` as a stack
overflow risk. On edge cases, it praised the dangling-`%` and unknown-specifier handling,
and flagged the unchecked `write()` return value as a real gap. On efficiency, it identified
the per-character `write()` calls as the core architectural weakness and suggested an
internal buffer, plus called out the recursion as adding avoidable overhead.

## Critical Evaluation

Sorting the feedback into buckets: a solid chunk of it was correct and useful - the missing
`write()` error check, the `print_binary`/`print_base` duplication, the `print_int`
signature inconsistency, and (this is my focus area) the complete absence of output
buffering are all real and verifiable by just reading the code. Some of it was correct but
generic - the memory safety section pads itself with "if buffers are added later, remember
to free them," which is true of any C project and isn't actually engaging with code that
doesn't allocate anything. The dispatch-table suggestion is reasonable C style advice in a
vacuum, but ignores that this is a novice, peer-graded school project where an array of
function pointers would probably be harder for a fellow student reviewer to follow than the
if/else chain that's there now - exactly the kind of "architectural constraint imposed by a
real project" the assignment brief warned AI tools miss.

The recursion-as-stack-overflow-risk claim is where the review actually goes wrong. `long`
tops out around 19-20 digits, so the recursion in `print_int`/`print_base` never goes deeper
than the teens - nowhere close to threatening an 8 MB default stack. Calling that "a real
robustness concern" is a generic warning applied without doing the one-line arithmetic
(digit count is bounded by `sizeof(long)`) that would have shown it doesn't apply here.

The `va_list`-by-value claim is the most interesting one, and I want to be honest that I'm
not fully certain where the line is. It's technically true per the standard that `va_list`
doesn't have to be a type that survives being passed by value across a function boundary.
But stated flatly as "may silently produce garbage or crash," with no mention that on
x86-64 Linux (this project's actual build target) `va_list` is an array type that decays to
a pointer, meaning this exact pattern is well-defined and is basically the standard idiom in
every teaching printf implementation - that's presenting a platform-dependent portability
caveat as an active bug. Trusting it without the missing context could have sent me chasing
a `va_copy` fix for something that isn't actually broken on the system that matters here.

The bigger miss, to me, was something the AI never brought up at all: `%d`/`%i` calls
`print_int(va_arg(args, int))`, implicitly converting the `int` to `long` before `print_int`
negates it. That widening is what protects against the classic `-INT_MIN` signed-overflow
bug - and it only actually protects you on a platform where `long` is wider than `int` (true
on Linux/LP64, not necessarily true on Windows/LLP64). That's a real, specific,
platform-dependent correctness question hiding in two lines of this code, and it never came
up, while a much less real "recursion might overflow the stack" line did. That contrast is
basically the whole finding of this project for me: the AI pattern-matches on categories of
code it's seen described before ("recursion," "va_list passed around") rather than working
out the actual bound or the actual value range for this specific file.

## Reflection on AI as a Reviewer

What it's good at: fast, broad-coverage sweeps. It caught real structural inconsistencies
(`print_binary` not reusing `print_base`, `print_int`'s odd signature) and a real edge-case
gap (unchecked `write()` return) that I hadn't consciously flagged before writing this up.
For a first pass over a codebase, that's genuinely useful - it's the kind of thing that
would otherwise take a slow manual read-through to notice.

What it's not good at, at least here: distinguishing a textbook-shaped risk from an actual
one. It knows "recursion can overflow the stack" and "va_list has UB pitfalls" as true
general statements about C, and it applies them without doing the specific math (max
recursion depth, target ABI) that would tell it whether either one actually applies to the
code in front of it. It also doesn't know anything about the project's real constraints -
that this is peer-graded, Betty-style, novice curriculum code, not a production library that
needs to scale or be maximally idiomatic - so "use a function pointer table" gets suggested
without weighing whether that's actually the right call here.

## Conclusion

I'd trust AI-assisted review for systems code as a first pass, not a final one. It's good at
surfacing things to go check - inconsistent signatures, duplicated logic, an unchecked
return value - faster than reading through the file line by line myself would be. But
anything it says about undefined behavior, stack safety, or portability needs to be verified
against the actual target platform and the actual value ranges involved before I'd act on
it, because it will state platform-dependent or bounded risks with the same confidence as
real ones. For a printf implementation specifically, where half the interesting bugs are
about exactly which platform's `long` width or exactly which ABI's `va_list` layout you're
running on, that's not a small gap - it's the difference between review and code review
theater.
