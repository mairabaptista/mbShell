# mbShell

mbShell is a simple simulation of a Linux shell.

## Installation

```bash
gcc -o shell shell.c utiities.c
```

## Usage

```bash
./shell
```
## Features
* Able to fork and work with child process.
* I/O redirection in the form of:
      `<cmd1> <argument> > <output file>`
      `<cmd1> <argument> < <input file>`
      `<cmd1> <argument> < <input file> > <output file>`
* Multiple pipes capability
      `<cmd1> <argument> | <cm2>`
* Mix and match I/O redirection with pipes
      `<cmd1> <argument> | <cmd2> <argument> | <cmd3> <argument> > <output file>`
* Background execution with `&`
