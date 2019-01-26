#!/bin/sh

if [ -f "$(which scell_seal_string_echo)" ]
then
    scell_seal_string_echo "$@"
else
    cargo run --quiet --example scell_seal_string_echo -- "$@"
fi
