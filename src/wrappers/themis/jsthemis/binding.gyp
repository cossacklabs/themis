{
  "targets": [
    {
      "target_name": "jsthemis",
      "sources": [
        "addon.cpp",
        "errors.cpp",
        "secure_message.cpp",
        "secure_keygen.cpp",
        "secure_session.cpp",
        "secure_cell_seal.cpp",
        "secure_cell_context_imprint.cpp",
        "secure_cell_token_protect.cpp",
        "secure_comparator.cpp",
      ],
      "include_dirs": [
         "<!(node -e \"require('nan')\")",
      ],
      "conditions": [
        [ "OS=='linux' or OS=='mac'", {
          "libraries": [
            "-L/usr/local/lib",
            "-L/usr/lib",
            "-lsoter",
            "-lthemis",
          ],
        }],
        [ "OS=='win'", {
          "libraries": [
            "libsoter.dll.a",
            "libthemis.dll.a",
          ],
          "msvs_settings": {
            "VCCLCompilerTool": {
              "AdditionalIncludeDirectories": [
                "<!(echo %ProgramFiles%)\\Themis\\include",
              ],
            },
            "VCLinkerTool": {
              "AdditionalLibraryDirectories": [
                "<!(echo %ProgramFiles%)\\Themis\\lib",
              ],
            },
          },
        }],
      ],
    }
  ]
}
