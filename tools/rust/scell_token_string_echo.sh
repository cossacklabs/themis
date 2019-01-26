#!/bin/sh

if [ -f "$(which scell_token_string_echo)" ]
then
    scell_token_string_echo "$@"
else
    cargo run --quiet --example scell_token_string_echo -- "$@"
fi
