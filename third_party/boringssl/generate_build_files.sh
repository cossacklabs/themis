#!/usr/bin/env bash
#
# Generate intermediate build files for Apple platforms.
#
# Usage:
#
#     third_party/boringssl/generate_build_files.sh
#
# Run from Themis source tree root.

set -eu

THIS_SCRIPT=third_party/boringssl/generate_build_files.sh

die() {
    for line in "$@"; do
        echo 2>&1 $line
    done
    exit 1
}

# First of all, check that we're being launched from Themis source tree root.
if [[ ! -f "$THIS_SCRIPT" ]]; then
    die "$PWD is not Themis source tree root." \
        "Please launch $THIS_SCRIPT from the root directory."
fi

# Make sure that all dependencies are in place. Give instructions in case
# not everything is installed. See BoringSSL's "BUILDING.md" for details.
if ! python2 --version >/dev/null 2>&1; then
    # Python 2 is currently bundled with macOS
    die 'Python 2 must be available as "python2"'
fi
if ! perl --version >/dev/null 2>&1; then
    # Perl is currently bundled with macOS
    die 'Perl must be available as "perl"'
fi
if ! go version >/dev/null 2>&1; then
    die 'Go must be available as "go"' \
        'Install it with "brew install go"'
fi

cd "$(dirname "$THIS_SCRIPT")"

# Clean the generated files so that if something is removed then we notice it.
rm -rf ios-* mac-* boringssl.gypi err_data.c

# Generate the intermediate files. BoringSSL is very... opinionated about
# the directory from which this utility is launched (third_party/boringssl),
# the name of the source directory of BoringSSL (must be "src" exactly),
# the output location (straight into third_party/boringssl with no options),
# and the script requires Python 2 and does not work with Python 3.
echo 2>&1 "Generating build files..."
python2 src/util/generate_build_files.py gyp
echo 2>&1 "Done!"

# Now, remove all the stuff that we don't need and don't use.
rm -rf linux-* win-* crypto_test_data.cc
