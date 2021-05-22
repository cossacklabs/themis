# ⏱ Rust Themis benchmarks

## Quickstart

Rust Themis benchmarks are written using [**Criterion.rs**](https://bheisler.github.io/criterion.rs/book/criterion_rs.html) statistical benchmarking tool.

The idea is to compare Themis Core benchmarks and RustThemis benchmarks to understand how much latency Rust wrapper adds.

You will need Rust toolchain installed to run benchmarks.
[Visit rustup.rs](https://rustup.rs/) to install Rust.

 1. 📦 **Install Themis Core**

    Benchmarks use Themis library from the system by default.
    <!-- TODO: use local builds if available -->

    Normally, this should be enough to install Themis:

    ```bash
    make install
    ```

    If it doesn’t work (or this is your first time building Themis)
    then you might need to [review the documentation](https://docs.cossacklabs.com/themis/installation/).

    If it still doesn’t work, please [file an issue](https://github.com/cossacklabs/themis/issues/new?labels=bug,installation,core&template=bug_report.md&title=).

 2. ⚙️ **Change directory from repository root**

    ```bash
    cd benches/rust
    ```

    It’s not required but you would have to type less.

 3. ⏳ **Build dependencies**

    ```bash
    cargo bench --no-run
    ```

    Criterion.rs has quite a few dependencies so be patient,
    you need to do this only once.

 4. 🚀 **Run some benchmarks**

    ```bash
    cargo bench -- "Secure Cell .* Seal, master key/64 KB"
    ```

    [See FAQ](#faq) for more information on how and what you can run.

 5. 📊 **Analyze result report**

    ```bash
    open target/criterion/report/index.html
    ```

    Done! 🎉

## Coverage

### Secure Cell

|                 | Master keys | Passphrases |
| --------------- | ----------- | ----------- |
| Seal            | ✅ complete | ✅ complete |
| Token Protect   | ✅ complete | 🛠 WIP       |
| Context Imprint | ✅ complete | ➖ N/A       |

### Secure Message

|                   | ECDSA      | RSA        |
| ----------------- | ---------- | ---------- |
| Encrypt / Decrypt | 💭 soon    | 💭 soon    |
| Sign / Verify     | 💭 soon    | 💭 soon    |

### Secure Session

💭 soon

### Secure Comparator

💭 soon

<!--

## Benchmark results

TODO: describe current benchmark results here

-->

## FAQ

First of all, it’s a good idea to familiarize yourself with
[Criterion.rs User Guide](https://bheisler.github.io/criterion.rs/book/criterion_rs.html),
especially sections on
[command-line options](https://bheisler.github.io/criterion.rs/book/user_guide/command_line_options.html),
[output format](https://bheisler.github.io/criterion.rs/book/user_guide/command_line_output.html),
and [interpreting results](https://bheisler.github.io/criterion.rs/book/analysis.html).

**Q:** What benchmarks are available?

```bash
cargo bench -- --list
```

**Q:** How do I run one of them?

```bash
cargo bench -- 'one of them'    # filter by regular expression
```

**Q:** How do I see if my optimizations have an effect?

```bash
git checkout feature
cargo bench -- --save-baseline feature-unoptimized

git checkout optimizations
# Work on performance
# ...

# Compare against the baseline version
cargo bench -- --baseline feature-unoptimized
```

Don’t forget to _reinstall_ Themis Core library every time you make a change in it and want to measure it.
<!-- TODO: and to pester maintainers to support local builds -->

**Q:** Benchmarking takes ages, what can I do?

```bash
cargo bench -- --sample-size 20     # cannot be lower than 10
```
