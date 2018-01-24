#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

echo ".. test js"
sh ./tests/_integration/integration_js.sh
check_result_zero

echo ".. test python"
sh ./tests/_integration/integration_python.sh
check_result_zero

echo ".. test php"
sh ./tests/_integration/integration_php.sh
check_result_zero

echo ".. test ruby"
sh ./tests/_integration/integration_ruby.sh
check_result_zero

echo ".. test go"
sh ./tests/_integration/integration_go.sh
check_result_zero

echo ".. test cell context imprint"
sh ./tests/_integration/integration_scell_context_imprint.sh
check_result_zero

echo ".. test cell seal"
sh ./tests/_integration/integration_scell_seal.sh
check_result_zero

echo ".. test cell token protect"
sh ./tests/_integration/integration_scell_token_protect.sh
check_result_zero

echo ".. test message encryption"
sh ./tests/_integration/integration_smessage_encryption.sh
check_result_zero

echo ".. test message signing"
sh ./tests/_integration/integration_smessage_signing.sh
check_result_zero

exit ${status}