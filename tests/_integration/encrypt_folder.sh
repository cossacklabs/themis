#!/bin/bash -e

HOST_NAME=$1

TEST_OUT=./tests/out/$HOST_NAME
mkdir -p $TEST_OUT

echo ".. encrypting data from $HOST_NAME in folder $TEST_OUT"

ruby ./tools/ruby/scell_seal_string_echo.rb "enc" "pass" "test seal: pass" > $TEST_OUT/scell_seal.txt
ruby ./tools/ruby/scell_seal_string_echo.rb "enc" "pass" "test seal context: pass" "somecontext" > $TEST_OUT/scell_seal_context.txt
ruby ./tools/ruby/scell_context_string_echo.rb "enc" "pass" "test context imprint: pass" "somecontext" > $TEST_OUT/scell_context_impr.txt
ruby ./tools/ruby/scell_token_string_echo.rb "enc" "pass" "test token: pass" > $TEST_OUT/scell_token.txt
ruby ./tools/ruby/scell_token_string_echo.rb "enc" "pass" "test token: pass" "somecontext" > $TEST_OUT/scell_token_context.txt