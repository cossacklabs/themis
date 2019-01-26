#!/bin/sh

if [ -f "$(which scell_context_string_echo)" ]
then
    scell_context_string_echo "$@"
else
    cargo run --quiet --example scell_context_string_echo -- "$@"
fi
