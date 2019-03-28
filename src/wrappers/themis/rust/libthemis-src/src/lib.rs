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

//! Building native Themis library.
//!
//! This crate can be used in `[build-dependencies]` for building Themis library in **build.rs**
//! for future inclusion into your Rust binaries as a static library.
//!
//! # Dependencies
//!
//! We expect all native Themis dependencies to be installed and correctly configured:
//!
//!   - C compiler
//!   - GNU Make
//!   - OpenSSL, LibreSSL, or BoringSSL
//!
//! Please refer to [the official documentation][docs] on installing and configuring dependencies.
//!
//! [docs]: https://github.com/cossacklabs/themis/wiki/Building-and-installing
//!
//! # Examples
//!
//! Typical usage from a `*-sys` crate looks like this:
//!
//! ```no_run
//! fn main() {
//!     #[cfg(feature = "vendored")]
//!     libthemis_src::make();
//!
//!     // Go on with your usual build.rs business, pkg_config crate
//!     // should be able to locate the local installation of Themis.
//!     // You'll probably need to use the static library.
//! }
//! ```

use std::env;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::{Command, Stdio};

// Compile-time check for that we really bundle Themis source code.
const _THEMIS_MAKEFILE: &[u8] = include_bytes!("../themis/Makefile");

/// A builder (literally!) for Themis, produces [`Library`].
///
/// [`Library`]: struct.Library.html
#[derive(Default)]
pub struct Build {
    out_dir: Option<PathBuf>,
}

/// Installed Themis library resulting from a [`Build`].
///
/// [`Build`]: struct.Build.html
pub struct Library {
    prefix: PathBuf,
}

/// Build and install Themis into the default location then tell **pkg-config** about it.
pub fn make() {
    Build::new().build().set_pkg_config_path();
}

/// Verifies binary dependencies of Themis build. Panics if dependencies are not satisfied.
fn check_dependencies() {
    fn fails_to_run(terms: &[&str]) -> bool {
        Command::new(&terms[0])
            .args(&terms[1..])
            .stdout(Stdio::null())
            .stderr(Stdio::null())
            .status()
            .is_err()
    }

    if fails_to_run(&["make", "--version"]) {
        panic!(
            "

It seems your system does not have GNU make installed. Make is required
to build Themis from source.

Please install \"make\" or \"build-essential\" package and try again.

        "
        );
    }

    if fails_to_run(&["cc", "--version"]) {
        panic!(
            "

It seems your system does not have a C compiler installed. C compiler
is required to build Themis from source.

Please install \"clang\" (or \"gcc\" and \"g++\") package and try again.

        "
        );
    }

    // TODO: check for SomethingSSL, it would be nice for the user
}

impl Build {
    /// Prepares a new build.
    pub fn new() -> Build {
        Build {
            out_dir: env::var_os("OUT_DIR").map(|s| PathBuf::from(s).join("themis")),
        }
    }

    /// Overrides output directory. Use it if OUT_DIR environment variable is not set or you want
    /// to customize the output location.
    pub fn out_dir<P: AsRef<Path>>(&mut self, path: P) -> &mut Self {
        self.out_dir = Some(path.as_ref().to_path_buf());
        self
    }

    /// Builds Themis, panics on any errors.
    pub fn build(&self) -> Library {
        check_dependencies();

        let out_dir = self.out_dir.as_ref().expect("OUT_DIR not set");
        let themis_src_dir = Path::new(env!("CARGO_MANIFEST_DIR")).join("themis");
        let themis_build_dir = out_dir.join("build");
        let themis_install_dir = out_dir.join("install");

        // Cargo requires build scripts to never write anything outside of OUT_DIR.
        // Take care to honor this requirement. It is checked by tools during builds.

        if !out_dir.exists() {
            fs::create_dir(&out_dir).expect("mkdir themis");
        }
        if themis_build_dir.exists() {
            fs::remove_dir_all(&themis_build_dir).expect("rm -r themis/build");
        }
        if themis_install_dir.exists() {
            fs::remove_dir_all(&themis_install_dir).expect("rm -r themis/install");
        }

        fs::create_dir(&themis_build_dir).expect("mkdir themis/build");
        fs::create_dir(&themis_install_dir).expect("mkdir themis/install");

        // Now we can build Themis and install it properly into OUT_DIR.
        let mut themis_build_and_install = make_cmd::make();
        themis_build_and_install
            .current_dir(&themis_src_dir)
            .stdout(Stdio::null())
            .env("BUILD_PATH", &themis_build_dir)
            .env("PREFIX", &themis_install_dir)
            .arg("install");

        // Cargo sets DEBUG environment variable to zero in release builds, but Themis build simply
        // checks existence of this variable. We need to unset it to get real release builds,
        if cfg!(debug) {
            themis_build_and_install.env("DEBUG", "1");
        } else {
            themis_build_and_install.env_remove("DEBUG");
        }

        let status = themis_build_and_install
            .status()
            .expect("failed to run Themis build");

        if !status.success() {
            panic!("Themis build failed: {}", status);
        }

        Library {
            prefix: themis_install_dir,
        }
    }
}

impl Library {
    /// Installation prefix of the Themis library.
    pub fn prefix(&self) -> &Path {
        &self.prefix
    }

    /// Adds installed Themis library location to PKG_CONFIG_PATH environment variable.
    pub fn set_pkg_config_path(&self) {
        let mut paths = env::var_os("PKG_CONFIG_PATH").unwrap_or_default();
        if !paths.is_empty() {
            paths.push(":");
        }
        paths.push(self.prefix.join("lib/pkgconfig"));

        env::set_var("PKG_CONFIG_PATH", paths);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    use std::env;
    use std::ffi::OsStr;

    #[test]
    fn build_and_install() {
        let temp_dir = tempfile::tempdir().expect("temporary directory");
        let library = Build::new().out_dir(&temp_dir).build();

        assert!(library.prefix().join("include/themis/themis.h").exists());
        assert!(library.prefix().join("lib/pkgconfig/libthemis.pc").exists());
        assert!(library.prefix().join("lib").read_dir().unwrap().count() > 0);
    }

    #[test]
    #[allow(non_snake_case)]
    fn build_and_install_to_OUT_DIR() {
        let temp_dir = tempfile::tempdir().expect("temporary directory");
        let library = with_env_var("OUT_DIR", temp_dir.path(), || Build::new().build());

        assert!(library.prefix().join("include/themis/themis.h").exists());
        assert!(library.prefix().join("lib/pkgconfig/libthemis.pc").exists());
        assert!(library.prefix().join("lib").read_dir().unwrap().count() > 0);
    }

    #[test]
    fn pkg_config_setting() {
        let temp_dir = tempfile::tempdir().expect("temporary directory");
        let library = Build::new().out_dir(&temp_dir).build();

        with_env_var("PKG_CONFIG_PATH", "", || {
            library.set_pkg_config_path();

            let pkg_path = env::var("PKG_CONFIG_PATH").expect("PKG_CONFIG_PATH");
            let prefix = library.prefix().to_str().expect("prefix").to_owned();
            assert!(pkg_path.contains(&prefix));
        });
    }

    fn with_env_var<K, V, F, T>(key: K, value: V, f: F) -> T
    where
        K: AsRef<OsStr>,
        V: AsRef<OsStr>,
        F: FnOnce() -> T,
    {
        let old_value = env::var_os(&key);
        env::set_var(&key, value);
        let result = f();
        match old_value {
            Some(old_value) => env::set_var(&key, old_value),
            None => env::remove_var(&key),
        }
        result
    }
}
