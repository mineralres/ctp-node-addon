// tick.js
var readline = require('readline');
var ctp = require('./ctp_business');
const addon = require('./build/Release/addon');
const iconv = require('iconv-lite');

let requestID = 0;

const api = new addon.TraderApi((t, d, errorID, errorMsg, requestID, isLast) => {
    if (errorID != 0 && errorMsg && Buffer.from(errorMsg)[0] != 0) {
        errorMsg = iconv.decode(Buffer.from(errorMsg), 'GBK');
    } else {
        errorMsg = ''
    }
    switch (t) {
        case 'OnFrontConnected': {
            console.log('交易服务器连接成功 OnFrontConnected')
            requestID += 1;
            let req = ctp.CThostFtdcReqUserLoginFieldInit();
            req.BrokerID = "9999";
            req.UserID = "143650";
            req.Password = "198612";
            const buff = new Uint8Array(ctp.CThostFtdcReqUserLoginFieldParser().encode(req));
            api.ReqUserLogin(buff, requestID);
        }
            break
        case 'OnRspUserLogin':
            {
                const pRspUserLogin = ctp.CThostFtdcRspUserLoginFieldParser().parse(Buffer.from(d))
                console.log('OnRspUserLogin', t, d.byteLength, errorID, errorMsg, requestID, isLast, pRspUserLogin)
                const req = ctp.CThostFtdcQryOrderFieldInit();
                req.BrokerID = '9999';
                requestID += 1;
                console.log(req);
                setTimeout(() => {
                  api.ReqQryOrder(
                    new Uint8Array(ctp.CThostFtdcQryOrderFieldParser().encode(req)),
                    requestID
                  );
                }, 1000);    
            }
            break
        case 'OnRtnInstrumentStatus':
            {
                const status = ctp.CThostFtdcInstrumentStatusFieldParser().parse(Buffer.from(d))
                // console.log('OnRtnInstrumentStatus', status)    
            }
            break
        default:
            console.log('On Message', t, d.byteLength)
    }
});

// addon.TraderApiCall(api, "RegisterFront", "tcp://180.168.146.187:10130"); // 全天站点
api.RegisterFront("tcp://180.168.146.187:10101");
api.SubscribePublicTopic(2);
api.SubscribePrivateTopic(2);
api.Init();
console.log('api = ', api, api.GetApiVersion())

// console.log(addon.hello());
var rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});

rl.on('line', function (line) {
    console.log(line);
})