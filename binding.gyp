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
    }
  ]
}
