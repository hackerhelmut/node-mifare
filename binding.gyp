{
  "targets": [
    {
      "target_name": "node_mifare",
      "dependencies": ["node_modules/libfreefare-pcsc/binding.gyp:freefare_pcsc"],
      "sources": [
        "src/mifare.cc",
        "src/reader.cc",
        "src/desfire.cc",
        "src/utils.cc"
      ],
      "cflags": [
        "-Wall",
        "-Wextra",
        "-Wno-unused-parameter",
        "-fPIC",
        "-fno-strict-aliasing",
        "-fno-exceptions",
        "-pedantic"
      ],

      "conditions": [
        ["OS=='linux'", {
          "include_dirs": ["/usr/include/PCSC"],
          "libraries": ["-lpthread", "-lpcsclite"]
        }],
        ["OS=='mac'", {
          "libraries": ["-framework", "PCSC"]
        }],
        ["OS=='win'", {
          "variables": {
            "freefare_path": "node_modules/libfreefare-pcsc",
            "openssl_root": "C:\OpenSSL-Win32"
          },
          "include_dirs": [
            "<(freefare_path)/contrib/win32"
            "<(freefare_path)/libfreefare",
            "C:\OpenSSL-Win32"
          ],
          "libraries": [
            "-lWinSCard",
            "-l<(openssl_root)/lib/libeay32.lib"
          ]
        }]
      ]
    }
  ]
}
