#!/bin/sh
#
# Copyright (c) 2019 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -eu

CLANG_FORMAT="${CLANG_FORMAT:-clang-format}"
CLANG_TIDY="${CLANG_TIDY:-clang-tidy}"
CFLAGS="${CFLAGS:-}"

FMT_MODE="${FMT_MODE:-fix}"

usage() {
    if [ $# -eq 1 ]
    then
        exec 2>&1
        echo "$1"
        echo
    fi
    cat <<EOF
usage:
    $0 [options] [--] <source-file> [marker-file]

options:
    -f, --fix       fix code style in-place (default)
    -c, --check     check code style with no changes
EOF
}

while [ $# -gt 0 ]
do
    case "$1" in
      -h|--help)
        usage
        exit
        ;;

      -f|--fix)
        FMT_MODE=fix
        shift
        ;;

      -c|--check)
        FMT_MODE=check
        shift
        ;;

      --)
        shift
        break
        ;;

      -*)
        usage "invalid option: $1"
        exit 1
        ;;

      *)
        break
        ;;
    esac
done

input=
marker=

case "$#" in
  1)
    input="$1"
    ;;
  2)
    input="$1"
    marker="$2"
    ;;
  *)
    usage "invalid argument count"
    exit 1
    ;;
esac

# clang-tidy likes to print useless warning statistics to stderr
# even with -quiet, so just ignore everything that goes there

case "$FMT_MODE" in
  fix)
    # Fix format after semantic fixes
    "$CLANG_TIDY" -fix "$input" -- $CFLAGS 2>/dev/null
    "$CLANG_FORMAT" -i "$input"
    if [ ! -z "$marker" ]
    then
        touch "$marker"
    fi
    ;;
  check)
    # Check format before semantic checks
    "$CLANG_FORMAT" "$input" | diff -u "$input" -
    "$CLANG_TIDY" "$input" -- $CFLAGS 2>/dev/null
    # Don't create marker file to allow fixing after checking
    ;;
esac
