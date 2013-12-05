{
  "targets": [
    {
      "variables": {
        "freefare_url": "https://github.com/hackerhelmut/libfreefare-pcsc.git",
        "freefarereader_mac": "https://gist.github.com/hackerhelmut/7798119/raw/f31cbc9382b5b22244191681b80044383da5860f/reader.h",
        "freefare_path": "build/freefare"
      },
      "target_name": "freefare_pcsc",
      "actions": [
        {
          "action_name": "fetch",
          "inputs": ["build"],
          "outputs": ["../<@(freefare_path)"],
          "action": ["git", "clone", "-b", "pcsc", "<@(freefare_url)", "<@(freefare_path)"],
          "message": "Fetching libfreefare for PCSC"
        },
        {
          "action_name": "make",
          "inputs": ["<@(freefare_path)/config.h"],
          "outputs": ["../<@(freefare_path)/libfreefare/.libs/libfreefare.a"],
          "action": ["make", "-C", "<@(freefare_path)"],
          "message": "Compiling libfreefare"
        }
      ],
      "conditions": [
        ["OS=='linux'", {
          "actions": [
            {
              "action_name": "configure",
              "inputs": ["<@(freefare_path)"],
              "outputs": ["../<@(freefare_path)/config.h"],
              "action": ["sh", "-c", "cd <(freefare_path); for x in 1 2; do automake --add-missing; libtoolize; autoreconf; done; ./configure --with-pcsc --with-pic"],
              "message": "Configuring libfreefare"
            }
          ]
        }],
        ["OS=='mac'", {
          "actions": [
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
            }
          ]
        }],
        ["OS=='win'", {}]
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
      "include_dirs": ["build/freefare/libfreefare"],
      "libraries": [
        "freefare/libfreefare/.libs/libfreefare.a"
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
          "libraries": ["-lWinSCard"]
        }]
      ]
    }
  ]
}
