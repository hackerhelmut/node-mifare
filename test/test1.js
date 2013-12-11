var mifare = require("../build/Release/node_mifare");
var ndef = require("ndef");

console.log(mifare);

var readers = mifare.getReader();
console.log(readers);

//readers["ACS ACR122U PICC Interface 00 00"].listen(function(err, reader, card) {
readers["ACS ACR122 0"].listen(function(err, reader, card) {
  card.setKey([0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], "aes", true, 1);
  card.setAid(0x542B);
  //console.log(err, reader, card);
  //console.log(card.info());
  //console.log(card.masterKeyInfo());
  //console.log(card.keyVersion(0));
  //console.log(card.freeMemory());
  //var read = card.readNdef();
  //console.log("Read:  ", read.data.ndef.toString('utf8'), read.data.ndef);
  
  var date = new Date();
  var date_str = date.toString();
  var ndef_msg = [
    ndef.mimeMediaRecord("akraja/pyrmont", date_str)
  ];

  var buffer = new Buffer(ndef.encodeMessage(ndef_msg));

  //buffer.write(date_str, 0, date_str.length, 'utf8');
  var write;
  for(var i = 0; i<10; i++) {
    console.log("Write", i);
    write = card.writeNdef(buffer);
    if((write && write.err && write.err.length == 0)) {
      break;
    }
  }
  if(write && write.err && write.err.length) {
    console.log(write);
  } else {
    console.log("Write: ", date_str, buffer);
  }

  var read;
  for(var i = 0; i<10; i++) {
    console.log("Read", i);
    read = card.readNdef();
    if((read && read.err && read.err.length == 0)) {
      break;
    }
  }
  if(read && read.err && read.err.length) {
    console.log(read);
  } else {
    console.log("Read:  ", read.data.ndef.toString('utf8'), read.data.ndef);
  }
  
});

