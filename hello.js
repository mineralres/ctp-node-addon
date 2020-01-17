// hello.js
// const addon = require('./build/Release/addon');

// console.log(addon.hello());
// Prints: 'world'

// Module import
var Parser = require("binary-parser-encoder").Parser;

// Build an IP packet header Parser
var ipHeader = new Parser()
  .endianess("big")
  .bit4("version")
  .bit4("headerLength")
  .uint8("tos")
  .uint16("packetLength")
  .uint16("id")
  .bit3("offset")
  .bit13("fragOffset")
  .uint8("ttl")
  .uint8("protocol")
  .uint16("checksum")
  .array("src", {
    type: "uint8",
    length: 4
  })
  .array("dst", {
    type: "uint8",
    length: 4
  });

// Prepare buffer to parse.
var buf = Buffer.from("450002c5939900002c06ef98adc24f6c850186d1", "hex");

// Parse buffer and show result
console.log(ipHeader.parse(buf));

var anIpHeader = {
  version: 4,
  headerLength: 5,
  tos: 0,
  packetLength: 709,
  id: 37785,
  offset: 0,
  fragOffset: 0,
  ttl: 44,
  protocol: 6,
  checksum: 61336,
  src: [ 173, 194, 79, 108 ],
  dst: [ 133, 1, 134, 209 ] };

// Encode an IP header object and show result as hex string
console.log(ipHeader.encode(anIpHeader).toString("hex"));