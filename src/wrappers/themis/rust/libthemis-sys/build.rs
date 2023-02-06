// Copyright 2018 (c) rust-themis developers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

fn main() {
    if std::env::var("DOCS_RS") == Ok("1".to_string()) {
        // Docs for https://docs.rs/themis are being generated, no need to
        // search for libthemis (it will fail anyway).
        return;
    }

    let mut pkg_config = pkg_config::Config::new();
    pkg_config.env_metadata(true);

    match pkg_config.probe("libthemis") {
        Ok(_) => {
            // pkg_config has already printed instructions for cargo.
            // We're done here.
        }
        Err(error) => {
            eprintln!(
                "
`libthemis-sys` could not find Themis installation in your system.

Please make sure you have appropriate development package installed.
On Linux it's called `libthemis-dev`, not just `libthemis`.
On macOS Homebrew formula is called `libthemis`.

Please refer to the documentation for installation instructions:

    https://github.com/cossacklabs/themis#quickstart

This crate uses `pkg-config` to locate the library. If you use
non-standard installation of Themis then you can help pkg-config
to locate your library by setting the PKG_CONFIG_PATH environment
variable to the path where `libthemis.pc` file is located.
"
            );
            eprintln!("{error}");

            if try_system_themis() {
                eprintln!(
                    "\
`libthemis-sys` tried using standard system paths and it seems that Themis
is available on your system. (However, pkg-config failed to find it.)
We will link against the system library.
"
                );
            } else {
                eprintln!(
                    "\
`libthemis-sys` also tried to use standard system paths, but without success.
It seems that Themis is really not installed in your system.
"
                );
                panic!("Themis Core not installed");
            }
        }
    }
}

fn try_system_themis() -> bool {
    let mut build = cc::Build::new();
    build.file("src/dummy.c");
    build.flag("-lthemis");

    let result = build.try_compile("dummy");
    if result.is_ok() {
        println!("cargo:rustc-link-lib=dylib=themis");
    }
    result.is_ok()
}
