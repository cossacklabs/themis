#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

echo ".. testing secure cell seal, node <--> node"
node ./tests/_integration/scell_seal_string_echo.js "dec" "passr" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passr" "node seal test"`
check_result_zero
echo ".. testing secure cell seal context, node <--> node"
node ./tests/_integration/scell_seal_string_echo.js "dec" "passr" `node ./tests/_integration/scell_seal_string_echo.js "enc" "passr" "node seal with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell context imprint, node <--> node"
node ./tests/_integration/scell_context_string_echo.js "dec" "passw2" `node ./tests/_integration/scell_context_string_echo.js "enc" "passw2" "node context with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell token protect, node <--> node"
node ./tests/_integration/scell_token_string_echo.js "dec" "passw1" `node ./tests/_integration/scell_token_string_echo.js "enc" "passw1" "node token test"`
check_result_zero
echo ".. testing secure cell token protect context, node <--> node"
node ./tests/_integration/scell_token_string_echo.js "dec" "passw2" `node ./tests/_integration/scell_token_string_echo.js "enc" "passw2" "node token with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure message encryption, node <--> node"
node ./tests/_integration/smessage_encryption.js "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node secure message"`
check_result_zero

echo ".. testing secure message signing, node <--> node"
node ./tests/_integration/smessage_encryption.js "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `node ./tests/_integration/smessage_encryption.js "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "node secure message"`
check_result_zero

exit ${status}