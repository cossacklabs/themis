american fuzzy lop
==================

[**american fuzzy lop**][afl]
is ~~a breed of rabbits~~ a security-oriented [fuzzer].
We use it to look for interesting execution paths in Themis:
crashes, hangs, unexpected behavior, etc.

[afl]: http://lcamtuf.coredump.cx/afl/
[fuzzer]: https://en.wikipedia.org/wiki/Fuzzing

## Quickstart

### Installing dependencies

 1. Install build dependencies for Themis.
    Please follow [these instructions][build].

 2. Install _american fuzzy lop_.
    It’s usually available in your system’s package repositories.
    If not, then [pay a visit here][afl] for build instructions.

    On Debian-like systems use:

    ```
    sudo apt-get install afl
    ```

    On macOS use:

    ```
    brew install afl-fuzz
    ```

That’s it.
You don’t have to compile and install Themis,
we’ll do it for you, in a special way.

[build]: https://github.com/cossacklabs/themis/wiki/Building-and-installing

### Running fuzzing tests

To fuzz something, do this:

```
make fuzz FUZZ_BIN=scell_seal_decrypt
```

For _scell_seal_decrypt_, one of the [available fuzzing tools] can be used.

This command prepares Themis for fuzzing,
builds all fuzzing tools,
and launches AFL fuzzer for the tool you've selected.
To stop fuzzing, press `Ctrl-C` in your terminal.

[available fuzzing tools]: input

### Environment variables

If the compilation fails,
you can try fixing it
by tweaking the following environment variables:

  - `AFL_FUZZ` —
    path to the fuzzer main binary
    (`afl-fuzz` from $PATH by default)

  - `AFL_CC` —
    path to the fuzzer instrumentation compiler
    (`afl-clang` from $PATH by default)

  - `AFL_CFLAGS` and `AFL_LDFLAGS` —
    additional flags for compiler and linker
    if you need them

### Analyzing results

Fuzzing results are put into the build directory.
The file layout is as follows:

```
build
└── afl
    ├── output                  results sorted by tool and execution date
    │   │
    │   ├── scell_seal_decrypt
    │   │   └── 2019-02-07_13-41-09
    │   │       ├── crashes
    │   │       │   └── ...     input files that cause crashes
    │   │       └── hangs
    │   │           └── ...     input files that cause hangs
    │   │
    │   └── scell_seal_roundtrip
    │       └── 2019-02-07_13-45-23
    │           ├── crashes
    │           │   └── ...
    │           └── hangs
    │               └── ...
    │
    ├── scell_seal_decrypt      fuzzing tool binaries
    └── scell_seal_roundtrip
```

You can use the provided tool to analyze the crashes:

```
./tools/afl/analyze_crashes.sh
```

By default, the tool reproduces the crashes
and prints a report with results and backtraces,
formatted as Markdown.
Run the tool with `--help` to learn more.

## Developing fuzzing tests

### Directory layout

Here you can see the following files:

  - [`README.md`](README.md) —
    you’re reading this file right now

  - [`fuzzy.mk`](fuzzy.mk) —
    a Makefile which describes how to build and run the fuzzing tests

  - [`analyze_crashes.sh`](analyze_crashes.sh) —
    a shell script producing a report for found crashes

  - [`src/`](src) —
    source code for all fuzzing tools lives here

  - [`input/`](input) —
    a directory with seed input data for each fuzzing test

  - [`generate/`](generate) —
    helper tools for humans to manually generate new seed data

Every fuzzing tool is identified by a directory in `input/$tool`,
which must contain at least one file with seed data for the tool.

There’s also a corresponding source file `src/$tool.c` for each tool,
which must contain the _main_() function for it.
Other files in `src` contain utility functions,
which will be available to all the tools.

### Adding new test tools

Here id what you need to do
in order to add a `${new_tool}` to fuzz test suite.

 1. Create `src/${new_tool}.c` file.

    It should be a simple C program,
    which accepts a single command-line argument:
    a path to the file with input data.
    The tool reads the file and
    exercises Themis in some way using the input data.
    If the test passes, it exits cleanly.

    The exit code does not matter.
    AFL will react to _abnormal_ behaviour of the tool,
    like crashes and hangups.
    For example, you can use _abort_() to signal
    assertion failures.

 2. Create `input/${new_tool}` directory.

 3. Add one or more seed tests to the input directory.

    Each file should be an example input for your tool
    that exercises a particular behavior.
    AFL will transform and mangle this input in various ways
    before passing it to the tool.

    You can name the files however you want.

    If the test data is not easy to write in a text editor,
    consider writing a generator tool
    to make it possible to reproduce and
    update the data later.

 4. Test your tool.

    ```
    make fuzz FUZZ_BIN=${new_tool}
    ```

### Writing good tools and seed tests

  - Each tool should exercise a single function of Themis.

    Don’t do too much in one tool, AFL is not _that_ smart.
    Limit the test to a single coherent piece of functionality
    that can break from malicious or malformed input.

  - Test data should be fairly small (less than 1 KB or so).

    AFL will try to minimize the input size
    so large examples will not win you anything.
    Use small inputs
    unless larger ones trigger a different behavior.

    The same goes for the test set diversity.
    You don‘t need to write a test for every possible error code.
    Focus on the general code paths 
    rather than on particular conditions.

  - Prefer binary data.

    AFL fuzzing techniques are based around binary transformations
    like bit flipping.
    It can work with text input just fine,
    but it is less likely
    to produce interesting results that way.

  - Keep the trust boundaries in mind.

    Any externally generated, user-provided
    input is a good place to fuzz.
    For example:
    key files,
    [Secure Cell](https://docs.cossacklabs.com/pages/secure-cell-cryptosystem/) containers,
    network packets received by [Secure Session](https://docs.cossacklabs.com/pages/secure-session-cryptosystem/),
    etc.

  - Use `abort()` to check assertions.

    AFL reacts to abnormal program termination.
    If you want to check if a particular condition holds,
    fail the test by calling `abort()` from your tool.
    The exit code is ignored by AFL.
