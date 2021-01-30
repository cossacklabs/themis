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

[build]: https://docs.cossacklabs.com/themis/installation/

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

## Caveats

### Fuzzing with sanitizers

Make sure to run `WITH_FATAL_SANITIZERS=yes`
so that sanitizers call abort() on errors instead of just printing messages.
AFL reacts to abnormal process terminations, not stderr traffic.

When building `WITH_ASAN`, we produce and test 32-bit binaries.
You will need to have 32-bit support installed
(e.g., `gcc-multilib`, `libc6-dev:i386`, `libssl-dev:i386` on Debian).
AFL cannot reliably work with address-sanitized 64-bit binaries
because ASAN allocates (but does not use) terabytes of virtual memory.
Unfortunately, AFL cannot distinguish that behavior from an allocation bug.
If you have to test 64-bit binaries, there are workarounds (like using cgroups on Linux).
Read AFL documentation to learn more
(`notes_for_asan.txt`, [mirror](https://github.com/mirrorer/afl/blob/master/docs/notes_for_asan.txt)).

Please note that glibc 2.25⁓2.27 has a bug that prevents ASAN from working with 32-bit binaries
[[1](https://github.com/google/sanitizers/issues/914),
 [2](https://github.com/google/sanitizers/issues/954),
 [3](https://sourceware.org/ml/libc-alpha/2018-02/msg00567.html),
 [4](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=84761)].
Currently, Clang does not have fixes applied so make sure to set `AFL_CC=afl-gcc`.

You may also need to bump the memory limit up a bit.
If AFL screams at you
```
[-] Whoops, the target binary crashed suddenly, before receiving any input
    from the fuzzer! Since it seems to be built with ASAN and you have a
    restrictive memory limit configured, this is expected; please read
    /usr/share/doc/afl-doc/docs/notes_for_asan.txt for help.
```
then try setting `AFL_FUZZ="afl-fuzz -m 1024"` to increase the limit to 1 GB,
that should be enough in most cases.

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

Here is what you need to do
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

    AFL will try to minimise the input size
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
    [Secure Cell](https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/) containers,
    network packets received by [Secure Session](https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-session/),
    etc.

  - Use `abort()` to check assertions.

    AFL reacts to abnormal program termination.
    If you want to check if a particular condition holds,
    fail the test by calling `abort()` from your tool.
    The exit code is ignored by AFL.
