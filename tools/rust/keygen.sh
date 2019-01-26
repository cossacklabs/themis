#!/bin/sh

if [ -f "$(which keygen_tool)" ]
then
    keygen_tool "$@"
else
    cargo run --quiet --example keygen_tool -- "$@"
fi
