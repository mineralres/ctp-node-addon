
#ifndef TRADE_XX_3399_
#define TRADE_XX_3399_

#include "./common.hpp"
#include "ctp6.3.15\\ThostFtdcTraderApi.h"
#include <iostream>
#include <mutex>
#include <node.h>
#include <thread>
#include <uv.h>
#include <v8.h>
#include <vector>

namespace addon {
using namespace v8;

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Object;
using v8::String;
using v8::Value;

template <typename T1>
Message *make_message(const char *type, T1 *t1,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) {
  return new Message(type, (const char *)t1, sizeof(T1), pRspInfo->ErrorID,
                     pRspInfo->ErrorMsg, nRequestID, bIsLast);
}

class TraderApi : public CThostFtdcTraderSpi {
public:
  void post(const char *type, const char *d, size_t n) {
    auto msg = new Message(type, d, n);
    post(msg);
  }

  void post(Message *msg) {
    TraderApi *data = (TraderApi *)async_.data;
    std::lock_guard<std::mutex> lock(data->mut_);
    data->msg_list_.push_back(msg);
    uv_async_send(&this->async_);
  }

  void OnFrontConnected() { post("OnFrontConnected", "", 0); };

  void OnFrontDisconnected(int nReason) {}
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) {
    post(make_message("OnRspUserLogin", pRspUserLogin, pRspInfo, nRequestID,
                      bIsLast));
  }

  uv_async_t async_;
  v8::Persistent<v8::Function> callback_;
  std::vector<Message *> msg_list_;
  CThostFtdcTraderApi *api_;
  std::mutex mut_;
};

// 创建交易API
void CreateTraderApi(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  CThostFtdcTraderApi *api = CThostFtdcTraderApi::CreateFtdcTraderApi();
  double x = *(double *)&api;
  args.GetReturnValue().Set(v8::Number::New(isolate, x));
  auto loop = uv_default_loop();
  TraderApi *spi = new TraderApi();
  spi->api_ = api;
  spi->async_.data = spi;
  api->RegisterSpi(spi);
  spi->callback_.Reset(isolate, Local<v8::Function>::Cast(args[0]));
  uv_async_init(loop, &spi->async_, onCtpMessage<TraderApi>);
}

// 调用TradeApi相关函数, 提供一个js回调函数接收消息
void TraderApiCall(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  double p = args[0]->NumberValue(isolate->GetCurrentContext()).ToChecked();
  int64_t px = *(int64_t *)&p;
  CThostFtdcTraderApi *api = (CThostFtdcTraderApi *)(px);
  v8::String::Utf8Value s(isolate, args[1]);
  std::string func(*s, s.length());
  if (func == "RegisterFront") {
    v8::String::Utf8Value s(isolate, args[2]);
    api->RegisterFront(*s);
  } else if (func == "Init") {
    api->Init();
  } else if (func == "ReqUserLogin") {
    auto ab = Local<v8::Uint8Array>::Cast(args[2])->Buffer();
    CThostFtdcReqUserLoginField *req =
        (CThostFtdcReqUserLoginField *)(ab->GetContents().Data());
    auto nRequestID =
        args[3]->Int32Value(isolate->GetCurrentContext()).ToChecked();
    api->ReqUserLogin(req, nRequestID);
  }
}

// 回收
void ReleaseTraderApi(const FunctionCallbackInfo<Value> &args) {}

} // namespace addon

#endif