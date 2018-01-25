#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

echo ".. testing secure cell seal, go <--> go"
go run ./tools/go/scell_seal_string_echo.go "dec" "passg" `go run ./tools/go/scell_seal_string_echo.go "enc" "passg" "go seal test"`
check_result_zero
echo ".. testing secure cell seal context, go <--> go"
go run ./tools/go/scell_seal_string_echo.go "dec" "passg" `go run ./tools/go/scell_seal_string_echo.go "enc" "passg" "go seal with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell context imprint, go <--> go"
go run ./tools/go/scell_context_string_echo.go "dec" "passg" `go run ./tools/go/scell_context_string_echo.go "enc" "passg" "go context with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell token protect, go <--> go"
go run ./tools/go/scell_token_string_echo.go "dec" "passg" `go run ./tools/go/scell_token_string_echo.go "enc" "passg" "go token test"`
check_result_zero
echo ".. testing secure cell token protect context, go <--> go"
go run ./tools/go/scell_token_string_echo.go "dec" "passg" `go run ./tools/go/scell_token_string_echo.go "enc" "passg" "go token with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure message encryption, go <--> go"
go run ./tools/go/smessage_encryption.go "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tools/go/smessage_encryption.go "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go secure message"`
check_result_zero

echo ".. testing secure message signing, go <--> go"
go run ./tools/go/smessage_encryption.go "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `go run ./tools/go/smessage_encryption.go "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "go secure message"`
check_result_zero

exit ${status}