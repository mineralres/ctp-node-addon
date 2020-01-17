// tick.js
var readline = require('readline');
var ctp = require('./ctp_business');
const addon = require('./build/Release/addon');
const iconv = require('iconv-lite');

let requestID = 0;

const api = addon.CreateTraderApi((t, d, errorID, errorMsg, requestID, isLast) => {
    if (errorID != 0 && errorMsg && Buffer.from(errorMsg)[0] != 0) {
        errorMsg = iconv.decode(Buffer.from(errorMsg), 'GBK');
    } else {
        errorMsg = ''
    }
    switch (t) {
        case 'OnFrontConnected':
            console.log('交易服务器连接成功 OnFrontConnected')
            requestID += 1;
            let req = ctp.CThostFtdcReqUserLoginFieldInit();
            req.BrokerID = "9999";
            req.UserID = "143650";
            req.Password = "198612";
            const buff = new Uint8Array(ctp.CThostFtdcReqUserLoginFieldParser().encode(req));
            addon.TraderApiCall(api, "ReqUserLogin", buff, requestID);
            break
        case 'OnRspUserLogin':
            const pRspUserLogin = ctp.CThostFtdcRspUserLoginFieldParser().parse(Buffer.from(d))
            console.log('OnRspUserLogin', t, d.byteLength, errorID, errorMsg, requestID, isLast, pRspUserLogin)
            break
        default:
            console.log('On Message ', t, d.byteLength)
    }
});

// addon.TraderApiCall(api, "RegisterFront", "tcp://180.168.146.187:10130");
addon.TraderApiCall(api, "RegisterFront", "tcp://180.168.146.187:10101");
addon.TraderApiCall(api, "Init");

// console.log(addon.hello());
var rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});

rl.on('line', function (line) {
    console.log(line);
})