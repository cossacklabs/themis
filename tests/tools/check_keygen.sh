#!/usr/bin/env bash

. tests/_integration/utils.sh

THEMIS_TEST_LANGUAGES="${THEMIS_TEST_LANGUAGES:-ruby,python,js,node,go,php,rust}"

test_lang() {
  [[ " $(echo "$THEMIS_TEST_LANGUAGES" | sed 's/,/ /g') " =~ " $1 " ]]
}

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

test_lang 'python' && test_keygen python3 tools/python/keygen.py
test_lang 'node' && test_keygen node tools/js/jsthemis/keygen.js
test_lang 'js' && test_keygen node tools/js/wasm-themis/keygen.js
test_lang 'php' && test_keygen "php -f" tools/php/keygen.php
test_lang 'ruby' && test_keygen ruby tools/ruby/keygen.rb
test_lang 'go' && test_keygen env tools/go/keygen.go.compiled
test_lang 'rust' && test_keygen env tools/rust/keygen_tool.rust

exit ${status}
