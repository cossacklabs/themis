#!/bin/bash -e

# import functions
. tests/_integration/utils.sh

echo ".. testing secure cell seal, ruby <--> ruby"
ruby ./tools/ruby/scell_seal_string_echo.rb "dec" "passr" `ruby ./tools/ruby/scell_seal_string_echo.rb "enc" "passr" "ruby seal test"`
check_result_zero
echo ".. testing secure cell seal context, ruby <--> ruby"
ruby ./tools/ruby/scell_seal_string_echo.rb "dec" "passr" `ruby ./tools/ruby/scell_seal_string_echo.rb "enc" "passr" "ruby seal with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell context imprint, ruby <--> ruby"
ruby ./tools/ruby/scell_context_string_echo.rb "dec" "passw2" `ruby ./tools/ruby/scell_context_string_echo.rb "enc" "passw2" "ruby context with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure cell token protect, ruby <--> ruby"
ruby ./tools/ruby/scell_token_string_echo.rb "dec" "passw1" `ruby ./tools/ruby/scell_token_string_echo.rb "enc" "passw1" "ruby token test"`
check_result_zero
echo ".. testing secure cell token protect context, ruby <--> ruby"
ruby ./tools/ruby/scell_token_string_echo.rb "dec" "passw2" `ruby ./tools/ruby/scell_token_string_echo.rb "enc" "passw2" "ruby token with context" "somecontext"` "somecontext"
check_result_zero

echo ".. testing secure message encryption, ruby <--> ruby"
ruby ./tools/ruby/smessage_encryption.rb "dec" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tools/ruby/smessage_encryption.rb "enc" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby secure message"`
check_result_zero

echo ".. testing secure message signing, ruby <--> ruby"
ruby ./tools/ruby/smessage_encryption.rb "verify" "./tests/_integration/keys/server.priv" "./tests/_integration/keys/client.pub" `ruby ./tools/ruby/smessage_encryption.rb "sign" "./tests/_integration/keys/client.priv" "./tests/_integration/keys/server.pub" "ruby secure message"`
check_result_zero

exit ${status}