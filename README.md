# Makefile Tutorial

Hello! This is intended to be a simple tutorial on how to use `make` for students currently suffering through their first class taught in the C programming language. This tutorial assumes you know how to use GCC and what C object files are. If you feel you already know the topic any particular section covers, feel free to skip it and read ahead.

## Index

- [Why do we Write Makefiles](#why-do-we-write-makefiles)
- [Basics of Makefiles](#basics-of-makefiles)
    - [Targets, Prerequisites, and Recipes](#targets-prerequisites-and-recipes)
    - [Variables, Automatic Variables, and Functions](#variables-automatic-variables-and-functions)
- [Expanding on Makefiles](#expanding-on-makefiles)
    - [Dependency Chains](#dependency-chains)
    - [Pattern Rules](#pattern-rules)
    - [Building an Executable with Pattern Rules](#building-an-executable-with-pattern-rules)

## Why do we Write Makefiles?

When I first learned how to use `make`, I questioned what it provided that bash scripts didn't. There are two very clear advantages to using `make`:

- **Will only rebuild parts of your program that have been modified.**

Imagine your program takes around five minutes to compile. Using `make`, you can avoid needing to wait five minutes for recompilation every time you make a change. It will only need to recompile the C source file you've made changes to.

- **`make` allows for flexible rules that can compile several parts of your program at once.**

I will explain this in detail later on, but what this means at a high-level is that you don't need one rule per C source file. In fact, under the right conditions you could create one Makefile that you can reuse for several C projects (assuming you structure them the same way).

## Basics of Makefiles

This section will roughly cover what you will learn about Makefiles in your first intro to C class.

### Targets, Prerequisites, and Recipes

---

To use `make`, create a file named `Makefile` in the root of your project.

Within a Makefile, you define rules. A rule will create a target file out of prerequisites using a recipe. This is the general layout of a rule:
```make
target: prerequisite(s)
    recipe
```

- **target**: This is the file you're trying to create. Often an executable.
- **prerequisite(s)**: File(s) required to create the target.
- **recipe**: The shell commands required to build the target.

To execute rule you typically call `make [target]`. You may also simply call `make` to execute the uppermost rule of your Makefile, or the `all` target if you've defined it.

Consider the following rule:

```make
prog: prog.c prog.h
    gcc prog.c -o prog
```

To execute this rule you would call `make prog`. If `prog` has been compiled previously, `make` will check if `prog.c` or `prog.h` have been edited. If either have, `prog` will be rebuilt.

### Variables, Automatic Variables, and Functions

---

Variables in Makefiles are fairly straightforward as the only data type is a string.

Consider the following Makefile:

```make
COMPILE=gcc
CFLAGS=-std=c99 -Wall -pedantic -g

prog: prog.c prog.h
    $(COMPILE) $(CFLAGS) $< -o $@
```

Let's break this down into its components.

`COMPILE` and `CFLAGS` are user-defined variables with their respective right-hand-side assigned to them. You can see in the recipe for `prog` that you can expand these variables by placing them in parentheses following a `$` character.

`$<` and `$@` are examples of automatic variables. Automatic variables have varying effect. I'll list ones regularly used below:
- `$@`: This expands the the target name.
- `$<`: This expands to the first prerequisite.
- `$^`: This expands to all prerequisites.

This list is nowhere near exhaustive. If you're interested in seeing all automatic variables, they're listed in the [official documentation](https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html).

Lastly, I'll cover how functions work in `make`. There are a variety of functions available (you can read about them [here](https://www.gnu.org/software/make/manual/html_node/Functions.html)), but for now I'll only cover the syntax.

Functions are called as follows:

```make
$(function_name arg_1, arg_2, ... arg_n)
```

The function must be within parentheses following a `$` character.

## Expanding on Makefiles

At this point we understand how to write rules by specifying a target, its prerequisites, and its recipe, as well as using variables and automatic variables. Below we'll cover the topics that turns `make` into a more powerful build system.

### Dependency Chains

---

Earlier we discussed that each rule has a series of prerequisites, and that these prerequisites determine whether or not the target is rebuilt on subsequent `make` calls. Prerequisites do more than just this though. Consider the following rules:

```make
prog: prog.o
    gcc $< -o $@

prog.o: prog.c prog.h
    gcc -c $< -o $@
```

Following exclusively what we've learned in the tutorial, we can see that `prog.o` is a prerequisite for `prog`. So our intuition might suggest that we should call `make prog.o` to build the prerequisite, then `make prog` to build the program. In reality, this isn't required.

Prerequisites serve another purpose. **When `make` sees a dependency, it will search for a rule to build that dependency automatically**. That means that in the following example, calling `make prog` will build successfully, even if `prog.o` hasn't been built yet, as the rule for `prog.o` is called automatically.

### Pattern Rules

---

In all the above examples, you still need to define individual rules for each target. Pattern rules allow you to get around this. To introduce pattern rules, I'll introduce a new character, `%`, which is somewhat like a wildcard. Consider this rule:

```make
%.o: %.c
    gcc -c $^ -o $@
```

What this means is that for any target with a `.o` extension, match it to corresponding prerequisite with a `.c` extension that has the same name.

All of this means that if we had the following tree structure:

```
./
├── add.c
├── add.h
├── main.c
├── Makefile
├── sub.c
└── sub.h
```

You could call `make add.o`, `make sub.o` or `make main.o` and due to the fact that they all have a corresponding C source file, they would all successfully build.

### Building an Executable with Pattern Rules

---

Please take a look within the example directory above, briefly looking at the `add.c`, `sub.c`, and `main.c` files. The following is the Makefile defined in the example directory:

```make
COMPILER=gcc -c
LINKER=gcc
CLFAGS=-Wall -pedantic -g

.PHONY: clean

OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))

main: $(OBJECTS)
	$(LINKER) $^ -o $@

%.o:: %.c %.h
	$(COMPILER) $(CLFAGS) $< -o $@

clean:
	rm -rf *.o
	rm main
```

There's quite a bit more going on in this example than in previous ones! I'll break this down line by line, ignoring the variables at the top.

1. `.PHONY: clean`

The `.PHONY` in this line specifies that the `clean` rule doesn't actually create a file. Imagine if you had a file named `clean` in your directory. `make` would detect it, and would always see that the file is up to date. This would prevent the `clean` rule from executing. By specifying it as a phony target, we avoid this issue.

2. `OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))`

`patsubst` is a string substitution function. It takes a pattern as the first argument, what to replace that pattern with as the second argument, and a string to apply it to as the third argument. `patsubst` expects the string to be a series of whitespace separated words, and the replacement is applied to each word in the string that matches. So if the function call was `$(patsubst %.c, %.o, file.c main.c)` the output would be `file.o main.o`. The function `wildcard` means that `*.c` will expand to every C source file in the directory as a whitespace separated string. This line allows me to create a variable that contains all of the C source files in the example directory with `.o` extensions instead of `.c` extensions.

3. `main: $(OBJECTS)`

This line defines the rule for creating objects files. In this case, `$(OBJECTS)` expands to `add.o sub.o main.o` which are all the prerequisites for the executable. You may be wondering why I'm not just using `%.o`. This is because the `%` character will not expand unless it is also used in the target. As the target has a static name, a variable is necessary.

4. `%o:: %.c %.h`

This is the last line I'd like to cover. This is the exact same as the rule in the pattern rule section, with one difference. It uses two colons instead of one. This is called a terminal rule. A terminal rule is a rule that doesn't allow `make` to search for ways to build the dependencies. This means that if `%.c` and `%.h` don't have matches in the example directory, `make` won't attempt to search for ways to build them.

Calling `make` or `make main` will build the example code.

---

With that, you should be able to create simple Makefiles. Hopefully this was helpful, and if you have any questions please reach out to me!
