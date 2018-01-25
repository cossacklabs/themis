#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

echo ".. testing secure cell seal, python <--> python"
python ./tools/python/scell_seal_string_echo.py "dec" "passr" `python ./tools/python/scell_seal_string_echo.py "enc" "passr" "python seal test"`
check_result_zero
echo ".. testing secure cell seal context, python <--> python"
python ./tools/python/scell_seal_string_echo.py "dec" "passr" `python ./tools/python/scell_seal_string_echo.py "enc" "passr" "python seal with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell context imprint, python <--> python"
python ./tools/python/scell_context_string_echo.py "dec" "passw2" `python ./tools/python/scell_context_string_echo.py "enc" "passw2" "python context with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell token protect, python <--> python"
python ./tools/python/scell_token_string_echo.py "dec" "passw1" `python ./tools/python/scell_token_string_echo.py "enc" "passw1" "python token test"`
check_result_zero
echo ".. testing secure cell token protect context, python <--> python"
python ./tools/python/scell_token_string_echo.py "dec" "passw2" `python ./tools/python/scell_token_string_echo.py "enc" "passw2" "python token with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure message encryption, python <--> python"
python ./tools/python/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tools/python/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python secure message"`
check_result_zero

echo ".. testing secure message signing, python <--> python"
python ./tools/python/smessage_encryption.py "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tools/python/smessage_encryption.py "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python secure message"`
check_result_zero

exit ${status}