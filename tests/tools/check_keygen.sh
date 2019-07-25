#!/usr/bin/env bash

. tests/_integration/utils.sh

test_keygen () {
    rm --force key key.pub test_key test_key.pub
    bash -c "$1 $2"
    check_result_zero
    stat key > /dev/null
    check_result_zero
    stat key.pub > /dev/null
    check_result_zero

    bash -c "$1 $2 test_key test_key.pub"
    check_result_zero
    stat test_key > /dev/null
    check_result_zero
    stat test_key.pub > /dev/null
    check_result_zero
}

test_keygen python tools/python/keygen.py
test_keygen node tools/js/jsthemis/keygen.js
test_keygen node tools/js/wasm-themis/keygen.js
test_keygen "php -f" tools/php/keygen.php
test_keygen ruby tools/ruby/keygen.rb
test_keygen "go run" tools/go/keygen.go
test_keygen env tools/rust/keygen_tool.rust

exit ${status}
