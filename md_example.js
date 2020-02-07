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
const api = new addon.MdApi((t, d) => {
    switch (t) {
        case 'OnFrontConnected':
            requestID += 1;
            let req = ctp.CThostFtdcReqUserLoginFieldInit();
            req.BrokerID = "9999";
            req.UserID = "059926";
            const buff = new Uint8Array(ctp.CThostFtdcReqUserLoginFieldParser().encode(req));
            api.ReqUserLogin(buff, requestID);
            break
        case 'OnRspUserLogin':
            api.SubscribeMarketData(["rb2005", "ru2005"]);
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
api.RegisterFront("tcp://180.168.146.187:10131");
api.Init();
console.log('api', api)

// console.log(addon.hello());
var rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});

rl.on('line', function (line) {
    console.log(line);
})