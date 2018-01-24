#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

echo ".. testing secure cell seal, python <--> python"
python ./tests/_integration/scell_seal_string_echo.py "dec" "passr" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passr" "python seal test"`
check_result_zero
echo ".. testing secure cell seal context, python <--> python"
python ./tests/_integration/scell_seal_string_echo.py "dec" "passr" `python ./tests/_integration/scell_seal_string_echo.py "enc" "passr" "python seal with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell context imprint, python <--> python"
python ./tests/_integration/scell_context_string_echo.py "dec" "passw2" `python ./tests/_integration/scell_context_string_echo.py "enc" "passw2" "python context with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell token protect, python <--> python"
python ./tests/_integration/scell_token_string_echo.py "dec" "passw1" `python ./tests/_integration/scell_token_string_echo.py "enc" "passw1" "python token test"`
check_result_zero
echo ".. testing secure cell token protect context, python <--> python"
python ./tests/_integration/scell_token_string_echo.py "dec" "passw2" `python ./tests/_integration/scell_token_string_echo.py "enc" "passw2" "python token with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure message encryption, python <--> python"
python ./tests/_integration/smessage_encryption.py "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tests/_integration/smessage_encryption.py "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python secure message"`
check_result_zero

echo ".. testing secure message signing, python <--> python"
python ./tests/_integration/smessage_encryption.py "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `python ./tests/_integration/smessage_encryption.py "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "python secure message"`
check_result_zero

exit ${status}