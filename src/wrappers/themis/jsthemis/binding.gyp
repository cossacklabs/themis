{
  "targets": [
    {
      "target_name": "jsthemis",
      "sources": [ "addon.cpp", "errors.cpp", "secure_message.cpp", "secure_keygen.cpp", "secure_session.cpp", "secure_cell_seal.cpp", "secure_cell_context_imprint.cpp", "secure_cell_token_protect.cpp", "secure_comparator.cpp" ],
      "libraries": ["-L/usr/local/lib/", "-L/usr/lib/", "-lsoter", "-lthemis"],
      "include_dirs": [
         "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
