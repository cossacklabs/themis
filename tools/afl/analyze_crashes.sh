#!/usr/bin/env sh
#
# Analyze crashes found by American Fuzzy Lop
#
# This tool is expected to be run with minimal configuration from the root
# directory of the repository:
#
#     ./tools/afl/analyze_crashes.sh
#
# Important environment variables:
#
#   - BUILD_PATH
#
#     Set this to the build path if you're using a non-standard one
#     (defaults to "build" in current directory)

set -u

BUILD_PATH="${BUILD_PATH:-build}"
INTERACTIVE="${INTERACTIVE:-no}"
DEBUGGER="${DEBUGGER:-}"

usage() {
    cat <<EOF
usage:

    $0 [options]

options:
    -h, --help          read this help
    -i, --interactive   drop into debugger for interactive investigation
                        instead of quietly producing an automated report
    -d, --debugger <path>
                        set the debugger to use, we support GDB and LLDB
        --no-debugger   disable backtrace printing, just check for failures
EOF
}

while [ $# -gt 0 ]
do
    case "$1" in
      -h|--help)
        usage
        exit
        ;;

      -i|--interactive)
        INTERACTIVE=yes
        shift
        ;;

      -d|--debugger)
        if [ $# -lt 2 ]
        then
            exec >&2
            echo "missing argument for $1"
            echo
            usage
            exit 1
        fi
        DEBUGGER="$2"
        shift 2
        ;;

      --no-debugger)
        DEBUGGER=/dev/null
        shift
        ;;

      *)
        exec >&2
        echo "invalid argument: $1"
        echo
        usage
        exit 1
        ;;
    esac
done

#
# Setup paths and check if there is anything to analyze
#

FUZZ_BIN_PATH="${BUILD_PATH}/afl"
FUZZ_THEMIS_PATH="${BUILD_PATH}/afl-themis"
FUZZ_OUTPUT_PATH="${FUZZ_BIN_PATH}/output"

if [ ! -d "$BUILD_PATH/afl" ]
then
    cat <<EOF >&2

It seems there are no fuzz testing reports in "$BUILD_PATH"

Run "make fuzz FUZZ_BIN=<binary>" to perform fuzz testing,
see "tools/afl/README.md" for more information.

EOF
    exit 1
fi

#
# Check the provided debugger (or try finding a usable one)
#

DEBUGGER_TYPE=

check_debugger() {
    if [ "$DEBUGGER" = "/dev/null" ]
    then
        DEBUGGER_TYPE=none
    elif "$DEBUGGER" --version 2>/dev/null | grep --quiet lldb
    then
        DEBUGGER_TYPE=lldb
    elif "$DEBUGGER" --version 2>/dev/null | grep --quiet gdb
    then
        DEBUGGER_TYPE=gdb
    else
        return 1
    fi
}

if ! check_debugger
then
    DEBUGGER=gdb
fi

if ! check_debugger
then
    DEBUGGER=lldb
fi

if ! check_debugger
then
    cat <<EOF >&2

Your system appears to not have a debugger installed.

We support gdb and lldb. Please install one of them to have
a better experience at analyzing crash dumps.

EOF
fi

#
# Crash dump analysis
#

analyze_crash() {
    local tool=$1
    local file=$2

    cat <<EOF
Run:

    $tool $file

Input (base64):

EOF
    # BSD and GNU have an ongoing feud over flag names...
    if base64 --wrap 64 </dev/null >/dev/null 2>/dev/null
    then
        cat "$file" | base64 --wrap 64
    else
        cat "$file" | base64 --break 64
    fi
    cat <<EOF

Debugger output:

EOF

    # Output Markdown-friendly text if we're reporting to file
    [ "$INTERACTIVE" = "no" ] && echo '```'

    case "$DEBUGGER_TYPE" in
      gdb)
        if [ "$INTERACTIVE" = "yes" ]
        then
            gdb --quiet --ex 'run' --args "$tool" "$file"
        else
            gdb --quiet --ex 'run' \
                --batch \
                --ex 'backtrace' \
                --ex 'disassemble /m $pc,+32' \
                --ex 'info reg' \
                --ex 'kill' \
                --ex 'quit' \
                --args "$tool" "$file" \
                2>&1
        fi
        ;;

      lldb)
        if [ "$INTERACTIVE" = "yes" ]
        then
            lldb -o "run" -- "$tool" "$file"
        else
            lldb --batch \
                -o "run" \
                -k "thread backtrace" \
                -k "disassemble --mixed --pc --count 8" \
                -k "register read" \
                -k "kill" \
                -k "quit" \
                -- "$tool" "$file" \
                2>&1
        fi
        ;;

      *)
        "$tool" "$file" || true
        ;;
    esac

    [ "$INTERACTIVE" = "no" ] && echo '```'
}

#
# Iterate through available crash findings and report them
#

print_banner=no
have_failures=no

for tool in $(ls "$FUZZ_BIN_PATH" 2>/dev/null)
do
    # The directory contains other files, we're interested in executables
    if ! ( [ -f "$FUZZ_BIN_PATH/$tool" ] && [ -x "$FUZZ_BIN_PATH/$tool" ] )
    then
        continue
    fi

    for run in $(ls "$FUZZ_OUTPUT_PATH/$tool" 2>/dev/null)
    do
        if [ "$print_banner" = "yes" ]
        then
            echo
        fi
        print_banner=no

        for crash in $(ls "$FUZZ_OUTPUT_PATH/$tool/$run/crashes" 2>/dev/null)
        do
            # The directory can contain README.txt and other files, skip them
            if [ ! -z "${crash%id*}" ]
            then
                continue
            fi

            if [ "$print_banner" = "no" ]
            then
                echo "# $tool -- $run"
                print_banner=yes
            fi

            crash_id=$(echo "$crash" | cut -d, -f1 | cut -d: -f2)
            signal_no=$(echo "$crash" | cut -d, -f2 | cut -d: -f2)
            signal_name=SIG$(kill -l $signal_no)

            echo
            echo "## id:$crash_id -- $signal_name"
            echo

            analyze_crash "$FUZZ_BIN_PATH/$tool" "$FUZZ_OUTPUT_PATH/$tool/$run/crashes/$crash"

            have_failures=yes
        done
    done
done

# Exit with non-zero status if we have printed a crash report
if [ "$have_failures" = "yes" ] && [ "$INTERACTIVE" = "no" ]
then
    exit 1
fi

exit
