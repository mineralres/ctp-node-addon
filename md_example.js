// tick.js
var readline = require('readline');
var ctp = require('./ctp_business');
const addon = require('./build/Release/addon');

const mdParser = ctp.CThostFtdcDepthMarketDataFieldParser();
let requestID = 0;
function toArrayBuffer(buf) {
    console.log(buf.buffer)
    var ab = new ArrayBuffer(buf.length);
    var view = new Uint8Array(ab);
    for (var i = 0; i < buf.length; ++i) {
        view[i] = buf[i];
    }
    return ab;
}
const mdapi = addon.CreateMdApi((t, d) => {
    switch (t) {
        case 'OnFrontConnected':
            requestID += 1;
            let req = ctp.CThostFtdcReqUserLoginFieldInit();
            req.BrokerID = "9999";
            req.UserID = "059926";
            req.Password = "198612";
            const buff = new Uint8Array(ctp.CThostFtdcReqUserLoginFieldParser().encode(req));
            addon.MdApiCall(mdapi, "ReqUserLogin", buff, requestID);
            break
        case 'OnRspUserLogin':
            addon.MdApiCall(mdapi, "SubscribeMarketData", ["rb2005", "ru2005"]);
            break
        case 'OnRtnDepthMarketData':
            const md = mdParser.parse(Buffer.from(d));
            console.log(t, md.UpdateTime, md.InstrumentID, md.ExchangeID, md.LastPrice, md.Volume);
            break
        default:
            console.log('On Message ', t, d.byteLength)
    }
});
// addon.MdApiCall(mdapi, "RegisterFront", "tcp://180.168.159.227:51213");
// addon.MdApiCall(mdapi, "RegisterFront", "tcp://180.168.146.187:10131");
addon.MdApiCall(mdapi, "RegisterFront", "tcp://180.168.146.187:10111");
addon.MdApiCall(mdapi, "Init");

// console.log(addon.hello());
var rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});

rl.on('line', function (line) {
    console.log(line);
})