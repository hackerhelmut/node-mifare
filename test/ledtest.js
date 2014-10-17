var mifare = require("../build/Release/node_mifare");
var ndef = require("ndef");

console.log(mifare);

var readers = mifare.getReader();
console.log(readers);

readers["ACS ACR122U PICC Interface 00 00"].listen(function(err, reader, card) {
  reader.setLed(0x0C, 0, 0, 0, 0);
});
