#!/bin/bash -e

TEST_IN=$1

echo ".. decrypting data from $TEST_IN"

ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "pass" `cat $TEST_IN/scell_seal.txt`
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "pass" `cat $TEST_IN/scell_seal_context.txt` "somecontext"
ruby ./tests/_integration/scell_context_string_echo.rb "dec" "pass" `cat $TEST_IN/scell_context_impr.txt` "somecontext"
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "pass" `cat $TEST_IN/scell_token.txt`
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "pass" `cat $TEST_IN/scell_token_context.txt` "somecontext"
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "pass" `cat $TEST_IN/scell_token_context.txt` "somecontext"