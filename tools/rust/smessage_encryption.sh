#!/bin/sh

if [ -f "$(which smessage_encryption)" ]
then
    smessage_encryption "$@"
else
    cargo run --quiet --example smessage_encryption -- "$@"
fi
