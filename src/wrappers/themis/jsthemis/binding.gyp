{
  "targets": [
    {
      "target_name": "jsthemis",
      "sources": [ "addon.cpp", "secure_message.cpp", "secure_keygen.cpp", "secure_session.cpp", "secure_cell_seal.cpp", "secure_cell_context_imprint.cpp", "secure_cell_token_protect.cpp", "secure_comparator.cpp" ],
      "libraries": ["/usr/lib/libthemis.a", "/usr/lib/libsoter.a"],
      "include_dirs": [
         "<!(node -e \"require('nan')\")"
      ],
      "defines": [
	"SECURE_COMPARATOR_ENABLED"
      ]
    }
  ]
}