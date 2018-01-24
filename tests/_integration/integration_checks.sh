#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

echo ".. testing secure cell, ruby <--> ruby"
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passr" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passr" "ruby seal test"`
check_result_zero
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passr" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passr" "ruby seal with context" "somecontext"` "somecontext"
check_result_zero

ruby ./tests/_integration/scell_context_string_echo.rb "dec" "passw2" `ruby ./tests/_integration/scell_context_string_echo.rb "enc" "passw2" "ruby context with context" "somecontext"` "somecontext"
check_result_zero

ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passw1" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passw1" "ruby token test"`
check_result_zero
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passw2" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passw2" "ruby token with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell, go <--> go"
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passg" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passg" "go seal test"`
check_result_zero
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passg" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passg" "go seal with context" "somecontext"` "somecontext"
check_result_zero

go run ./tests/_integration/scell_context_string_echo.go "dec" "passg" `go run ./tests/_integration/scell_context_string_echo.go "enc" "passg" "go context with context" "somecontext"` "somecontext"
check_result_zero

go run ./tests/_integration/scell_token_string_echo.go "dec" "passg" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passg" "go token test"`
check_result_zero
go run ./tests/_integration/scell_token_string_echo.go "dec" "passg" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passg" "go token with context" "somecontext"` "somecontext"
check_result_zero


echo ".. testing SECURE CELL, SEAL MODE, go <--> ruby"
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passw1" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passw1" "go-ruby test"`
check_result_zero
ruby ./tests/_integration/scell_seal_string_echo.rb "dec" "passw2" `go run ./tests/_integration/scell_seal_string_echo.go "enc" "passw2" "go-ruby with context" "somecontext"` "somecontext"
check_result_zero

go run ./tests/_integration/scell_seal_string_echo.go "dec" "passw3" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passw3" "ruby-go test"`
check_result_zero
go run ./tests/_integration/scell_seal_string_echo.go "dec" "passw4" `ruby ./tests/_integration/scell_seal_string_echo.rb "enc" "passw4" "ruby-go with context" "somecontext"` "somecontext"
check_result_zero


echo ".. testing SECURE CELL, CONTEXT IMPRINT MODE, go <--> ruby"
ruby ./tests/_integration/scell_context_string_echo.rb "dec" "passw2" `go run ./tests/_integration/scell_context_string_echo.go "enc" "passw2" "go-ruby with context" "somecontext"` "somecontext"
check_result_zero
go run ./tests/_integration/scell_context_string_echo.go "dec" "passw4" `ruby ./tests/_integration/scell_context_string_echo.rb "enc" "passw4" "ruby-go with context" "somecontext"` "somecontext"
check_result_zero


echo ".. testing SECURE CELL, TOKEN PROTECT MODE, go <--> ruby"
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passw1" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passw1" "go-ruby test"`
check_result_zero
ruby ./tests/_integration/scell_token_string_echo.rb "dec" "passw2" `go run ./tests/_integration/scell_token_string_echo.go "enc" "passw2" "go-ruby with context" "somecontext"` "somecontext"
check_result_zero

go run ./tests/_integration/scell_token_string_echo.go "dec" "passw3" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passw3" "ruby-go test"`
check_result_zero
go run ./tests/_integration/scell_token_string_echo.go "dec" "passw4" `ruby ./tests/_integration/scell_token_string_echo.rb "enc" "passw4" "ruby-go with context" "somecontext"` "somecontext"
check_result_zero

exit ${status}