#!/usr/bin/env bash
#
# Merge multiple static libraries into one.
#
#     scripts/merge-static-libs.sh libtarget.a [libdep1.a ...]
#
# Libraries libdep1.a are merged into libtarget.a. That is, all object files
# from those libraries are copied into the target library.
#
# Note that no deduplication is performed so use this script only once.

set -euo pipefail

AR=${AR:-ar}

help() {
    cat $0 | awk 'NR == 3, /^$/ { print substr($0, 3) }'
}

while [[ $# -gt 0 ]]
do
    case "$1" in
      -h|--help) help; exit;;
      --) shift; break;;
      -*)
        echo >&2 "Unknown option: $1"
        echo >&2
        help
        exit 1
        ;;
      *)
        break
        ;;
    esac
done

if [[ $# -lt 1 ]]
then
    echo >&2 "Invalid command-line: missing target library name"
    echo >&2
    help
    exit 1
fi

target="$1"
shift

tempdir="$(mktemp -d)"
trap 'rm -rf "$tempdir"' EXIT

while [[ $# -gt 0 ]]
do
    if [[ ! -f "$1" ]]
    then
        echo >&2 "No such file: $1"
        exit 1
    fi
    # Cast a possibly relative path to the absolute one.
    src="$(cd "$(dirname "$1")" >/dev/null 2>&1 && pwd)/$(basename "$1")"
    cd "$tempdir"
    "$AR" x "$src"
    cd "$OLDPWD"
    # Actually merge the contents of the library into the target one.
    find "$tempdir" -type f -name '*.o' -print0 | xargs -0 "$AR" rs "$target"
    # Clean up and go on.
    rm -rf "$tempdir"/*
    shift
done
