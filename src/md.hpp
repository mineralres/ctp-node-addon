
#include "./common.hpp"
#include "ctp6.3.15\\ThostFtdcMdApi.h"
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

class MdApi : public CThostFtdcMdSpi {
public:
  void post(const char *type, const char *d, size_t n) {
    // std::cout << " call post " << type
    //           << " SIZE=" << sizeof(CThostFtdcDepthMarketDataField)
    //           << std::endl;
    MdApi *data = (MdApi *)async_.data;
    auto msg = new Message(type, d, n);
    std::lock_guard<std::mutex> lock(data->mut_);
    data->msg_list_.push_back(msg);
    uv_async_send(&this->async_);
  }

  void OnFrontConnected() { post("OnFrontConnected", "", 0); };

  void OnFrontDisconnected(int nReason) {}
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) {
    post("OnRspUserLogin", "", 0);
  }

  void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
    post("OnRtnDepthMarketData", (const char *)pDepthMarketData,
         sizeof(CThostFtdcDepthMarketDataField));
  }

  uv_async_t async_;
  v8::Persistent<v8::Function> callback_;
  std::vector<Message *> msg_list_;
  CThostFtdcMdApi *api_;
  std::mutex mut_;
};

// 创建行情API
void CreateMdApi(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  CThostFtdcMdApi *mdapi = CThostFtdcMdApi::CreateFtdcMdApi();
  double x = *(double *)&mdapi;
  args.GetReturnValue().Set(v8::Number::New(isolate, x));
  auto loop = uv_default_loop();
  MdApi *spi = new MdApi();
  spi->api_ = mdapi;
  spi->async_.data = spi;
  mdapi->RegisterSpi(spi);
  spi->callback_.Reset(isolate, Local<v8::Function>::Cast(args[0]));
  uv_async_init(loop, &spi->async_, onCtpMessage<MdApi>);
}

// 调用MdApi相关函数, 提供一个js回调函数接收消息
void MdApiCall(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  double p = args[0]->NumberValue(isolate->GetCurrentContext()).ToChecked();
  int64_t px = *(int64_t *)&p;
  CThostFtdcMdApi *mdapi = (CThostFtdcMdApi *)(px);
  v8::String::Utf8Value s(isolate, args[1]);
  std::string func(*s, s.length());
  if (func == "RegisterFront") {
    v8::String::Utf8Value s(isolate, args[2]);
    // std::string front(*s, s.length());
    mdapi->RegisterFront(*s);
  } else if (func == "Init") {
    mdapi->Init();
  } else if (func == "ReqUserLogin") {
    auto ab = (Local<v8::Uint8Array>::Cast(args[2]))->Buffer();
    CThostFtdcReqUserLoginField *req =
        (CThostFtdcReqUserLoginField *)(ab->GetContents().Data());
    auto nRequestID =
        args[3]->Int32Value(isolate->GetCurrentContext()).ToChecked();
    mdapi->ReqUserLogin(req, nRequestID);
  } else if (func == "SubscribeMarketData") {
    v8::Local<Array> array = Local<Array>::Cast(args[2]);
    auto instruments = new char *[array->Length()];
    for (int i = 0; i < array->Length(); i++) {
      instruments[i] = *v8::String::Utf8Value(
          isolate,
          array->Get(isolate->GetCurrentContext(), i).ToLocalChecked());
    }
    mdapi->SubscribeMarketData(instruments, array->Length());
    delete instruments;
  }
}

// 回收
void ReleaseMdApi(const FunctionCallbackInfo<Value> &args) {}

} // namespace addon