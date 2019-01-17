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

use std::env;
use std::path::{Path, PathBuf};

use pkg_config::Library;

fn main() {
    let themis = get_themis();

    let whitelist = "(THEMIS|themis|secure_(comparator|session)|STATE)_.*";
    let bindings = bindgen::Builder::default()
        .clang_args(clang_include_paths(&themis))
        .clang_args(clang_library_paths(&themis))
        .header("src/wrapper.h")
        .whitelist_function(whitelist)
        .whitelist_type(whitelist)
        .whitelist_var(whitelist)
        .rustified_enum("themis_key_kind")
        .generate()
        .expect("generating bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("writing bindings!");

    cc::Build::new()
        .file("src/wrapper.c")
        .include("src")
        .includes(&themis.include_paths)
        .compile("themis_shims");
}

/// Embarks on an incredible adventure and returns with a suitable Themis (or dies trying).
fn get_themis() -> Library {
    #[cfg(feature = "vendored")]
    libthemis_src::make();

    let mut pkg_config = pkg_config::Config::new();
    pkg_config.env_metadata(true);
    pkg_config.arg("libsoter"); // TODO: remove this together with themis_shims

    #[cfg(feature = "vendored")]
    pkg_config.statik(true);

    match pkg_config.probe("libthemis") {
        Ok(library) => return library,
        Err(error) => panic!(format!(
            "

`libthemis-sys` could not find Themis installation in your system.

Please make sure you have appropriate development package installed.
On Linux it's called `libthemis-dev`, not just `libthemis`.
On macOS Homebrew formula is called `themis` or `themis-openssl`.

Please refer to the documentation for installation instructions:

    https://github.com/cossacklabs/themis#quickstart

This crate uses `pkg-config` to locate the library. If you use
non-standard installation of Themis then you can help pkg-config
to locate your library by setting the PKG_CONFIG_PATH environment
variable to the path where `libthemis.pc` file is located.

{}
",
            error
        )),
    }
}

fn clang_include_paths(library: &Library) -> Vec<String> {
    library
        .include_paths
        .iter()
        .map(|p| format!("-I{}", p.display()))
        .collect()
}

fn clang_library_paths(library: &Library) -> Vec<String> {
    library
        .link_paths
        .iter()
        .map(|p| format!("-L{}", p.display()))
        .collect()
}

trait CCBuildEx {
    fn includes<I>(&mut self, dirs: I) -> &mut Self
    where
        I: IntoIterator,
        I::Item: AsRef<Path>;
}

impl CCBuildEx for cc::Build {
    fn includes<I>(&mut self, dirs: I) -> &mut Self
    where
        I: IntoIterator,
        I::Item: AsRef<Path>,
    {
        for dir in dirs {
            self.include(dir);
        }
        self
    }
}
