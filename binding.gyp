{
  "variables": {
    "freefare_url": "https://github.com/hackerhelmut/libfreefare-pcsc.git",
    "freefarereader_mac": "https://gist.github.com/hackerhelmut/7798119/raw/f31cbc9382b5b22244191681b80044383da5860f/reader.h",
    "freefare_path": "build/freefare",
    "openssl_root": "C:\OpenSSL-Win32"
  },
  "conditions": [
  ],
  "targets": [
    {
      "target_name": "freefare_pcsc",
      "dependencies": ["openssl"],
      "actions": [
      ],
      "conditions": [
        ["OS=='linux'", {
          "actions": [
            {
              "action_name": "fetch",
              "inputs": ["build"],
              "outputs": ["../<@(freefare_path)"],
              "action": ["git", "clone", "-b", "pcsc", "<@(freefare_url)", "<@(freefare_path)"],
              "message": "Fetching libfreefare for PCSC"
            },
            {
              "action_name": "configure",
              "inputs": ["<@(freefare_path)"],
              "outputs": ["../<@(freefare_path)/config.h"],
              "action": ["sh", "-c", "cd <(freefare_path); for x in 1 2; do automake --add-missing; libtoolize; autoreconf; done; ./configure --with-pcsc --with-pic"],
              "message": "Configuring libfreefare"
            },
            {
              "action_name": "make",
              "inputs": ["<@(freefare_path)/config.h"],
              "outputs": ["../<@(freefare_path)/libfreefare/.libs/libfreefare.a"],
              "action": ["make", "-C", "<@(freefare_path)"],
              "message": "Compiling libfreefare"
            }
          ]
        }],
        ["OS=='mac'", {
          "actions": [
            {
              "action_name": "fetch",
              "inputs": ["build"],
              "outputs": ["../<@(freefare_path)"],
              "action": ["git", "clone", "-b", "pcsc", "<@(freefare_url)", "<@(freefare_path)"],
              "message": "Fetching libfreefare for PCSC"
            },
            {
              "action_name": "reader",
              "inputs": ["<@(freefare_path)"],
              "outputs": ["../<@(freefare_path)/libfreefare/reader.h"],
              "action": ["wget", "<@(freefarereader_mac)", "-O", "<@(freefare_path)/libfreefare/reader.h"],
              "message": "Fetching libpcsc-lite reader.h for MacOS"
            },
            {
              "action_name": "configure",
              "inputs": ["<@(freefare_path)", "<@(freefare_path)/libfreefare/reader.h"],
              "outputs": ["../<@(freefare_path)/config.h"],
              "action": ["sh", "-c", "cd <(freefare_path); ln -s /usr/local/share/libtool/config/ltmain.sh .; autoreconf -vis; export PCSC_CFLAGS=\"-framework PCSC\"; export PCSC_LIBS=\"-framework PCSC\"; ./configure --with-pcsc"],
              "message": "Configuring libfreefare"
            },
            {
              "action_name": "make",
              "inputs": ["<@(freefare_path)/config.h"],
              "outputs": ["../<@(freefare_path)/libfreefare/.libs/libfreefare.a"],
              "action": ["make", "-C", "<@(freefare_path)"],
              "message": "Compiling libfreefare"
            }
          ]
        }],
        ["OS=='win'", {
            "target_name": "freefare_pcsc",
            "product_prefix": "lib",
            "type": "static_library",
            "defines": [
              "HAVE_PCSC"
            ],
            "variables": {
              "freefare_src": "libfreefare/libfreefare",
              "freefare_config": "libfreefare/",
              "freefare_win": "libfreefare/contrib/win32"
            },
            "sources": [
              "<(freefare_src)/freefare.c",
              "<(freefare_src)/freefare.h",
              "<(freefare_src)/freefare_pcsc.h",
              "<(freefare_src)/freefare_nfc.h",
              "<(freefare_src)/freefare_internal.h",
              "<(freefare_src)/mad.c",
              "<(freefare_src)/mifare_application.c",
              "<(freefare_src)/mifare_classic.c",
              "<(freefare_src)/mifare_desfire.c",
              "<(freefare_src)/mifare_desfire_aid.c",
              "<(freefare_src)/mifare_desfire_crypto.c",
              "<(freefare_src)/mifare_desfire_error.c",
              "<(freefare_src)/mifare_desfire_key.c",
              "<(freefare_src)/mifare_ultralight.c",
              "<(freefare_src)/tlv.c"
            ],
            "cflags": [
              "-Wall",
              "-Wextra",
              "-Wno-unused-parameter",
              "-fPIC",
              "-fno-strict-aliasing",
              "-fno-exceptions",
              "-pedantic",
              "-UHAVE_LIBNFC"
            ],
            'msvs_settings': {
              'VCCLCompilerTool': {
                'AdditionalOptions': [
                  '/TP',
                ],
              },
            },
            "include_dirs": ["<(freefare_win)", "<(freefare_src)", "<(freefare_config)", "<(openssl_root)/include"],
            "libraries": [
            ]
        }]
      ]
    },
    {
      "target_name": "node_mifare",
      "dependencies": ["freefare_pcsc"],
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
        ["OS!='win'", {
          "include_dirs": ["build/freefare/libfreefare"],
          "libraries": [
            "freefare/libfreefare/.libs/libfreefare.a"
          ],
        }],
        ["OS=='linux'", {
          "include_dirs": ["/usr/include/PCSC"],
          "libraries": ["-lpthread", "-lpcsclite"]
        }],
        ["OS=='mac'", {
          "libraries": ["-framework", "PCSC"]
        }],
        ["OS=='win'", {
          "include_dirs": [
            "libfreefare/libfreefare",
            "libfreefare/contrib/win32"
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
