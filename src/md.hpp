
#include "./common.hpp"
#include "ctp6.3.15\\ThostFtdcMdApi.h"
#include <assert.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace Napi;

namespace addon {
class MdApi : public CThostFtdcMdSpi, public Napi::ObjectWrap<MdApi> {
public:
  void callJS(Message *msg) {
    napi_status status = tsfn_.BlockingCall(
        msg, [](Napi::Env env, Function jsCallback, Message *msg) {
          if (msg->data_.size() > 0) {
            jsCallback.Call({
                String::New(env, msg->type_),
                Napi::ArrayBuffer::New(env, (void *)msg->data_.data(),
                                       msg->data_.size()),
            });
          } else {
            jsCallback.Call({String::New(env, msg->type_)});
          }
          delete msg;
        });
    if (status != napi_ok) {
    }
  }

  static Napi::Object MdApi::Init(Napi::Env env, Napi::Object exports) {
    static Napi::FunctionReference constructor;
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "MdApi",
        {
            InstanceMethod("GetApiVersion", &MdApi::GetApiVersion),
            InstanceMethod("Release", &MdApi::Release),
            InstanceMethod("Init", &MdApi::Init),
            InstanceMethod("Join", &MdApi::Join),
            InstanceMethod("GetTradingDay", &MdApi::GetTradingDay),
            InstanceMethod("RegisterFront", &MdApi::RegisterFront),
            InstanceMethod("RegisterNameServer", &MdApi::RegisterNameServer),
            InstanceMethod("RegisterFensUserInfo",
                           &MdApi::RegisterFensUserInfo),
            InstanceMethod("SubscribeMarketData", &MdApi::SubscribeMarketData),
            InstanceMethod("UnSubscribeMarketData",
                           &MdApi::UnSubscribeMarketData),
            InstanceMethod("SubscribeForQuoteRsp",
                           &MdApi::SubscribeForQuoteRsp),
            InstanceMethod("UnSubscribeForQuoteRsp",
                           &MdApi::UnSubscribeForQuoteRsp),
            InstanceMethod("ReqUserLogin", &MdApi::ReqUserLogin),
            InstanceMethod("ReqUserLogout", &MdApi::ReqUserLogout),

        });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("MdApi", func);
    return exports;
  }

  // 创建行情API
  MdApi::MdApi(const CallbackInfo &info) : Napi::ObjectWrap<MdApi>(info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
      throw TypeError::New(env, "Expected two arguments");
    } else if (!info[0].IsFunction()) {
      throw TypeError::New(env, "Expected first arg to be function");
    }

    api_ = CThostFtdcMdApi::CreateFtdcMdApi();
    api_->RegisterSpi(this);
    tsfn_ = ThreadSafeFunction::New(
        env,
        info[0].As<Function>(), // JavaScript function called asynchronously
        "Resource Name",        // Name
        0,                      // Unlimited queue
        1,                      // Only one thread will use this initially
        [](Napi::Env) {         // Finalizer used to clean threads up
          // nativeThread.join();
        });
  }

  ~MdApi() { std::cout << "MdApi Destructor " << std::endl; }

private:
  Napi::ThreadSafeFunction tsfn_;
  CThostFtdcMdApi *api_;
  // 以下是生成js call Api的方法----------------------
  Napi::Value GetApiVersion(const CallbackInfo &info) {
    return Napi::String::New(info.Env(), api_->GetApiVersion());
  }

  void Release(const CallbackInfo &info) { api_->Release(); }

  void Init(const CallbackInfo &info) { api_->Init(); }

  void Join(const CallbackInfo &info) { api_->Join(); }

  Napi::Value GetTradingDay(const CallbackInfo &info) {
    return Napi::String::New(info.Env(), api_->GetTradingDay());
  }

  void RegisterFront(const CallbackInfo &info) {
    std::string str = info[0].As<Napi::String>();
    api_->RegisterFront((char *)str.data());
  }

  void RegisterNameServer(const CallbackInfo &info) {
    std::string str = info[0].As<Napi::String>();
    api_->RegisterNameServer((char *)str.data());
  }

  void RegisterFensUserInfo(const CallbackInfo &info) {
    auto ab = info[0].As<Napi::TypedArray>().ArrayBuffer();
    CThostFtdcFensUserInfoField *req = (CThostFtdcFensUserInfoField *)ab.Data();
    api_->RegisterFensUserInfo(req);
  }

  Napi::Value SubscribeMarketData(const CallbackInfo &info) {
    auto arr = info[0].As<Napi::Array>();
    auto instruments = new char *[arr.Length()];
    for (size_t i = 0; i < arr.Length(); i++) {
      Napi::Value v = arr[i];
      std::string str = v.As<Napi::String>();
      instruments[i] = new char[str.size()];
      memcpy(instruments[i], str.c_str(), str.size());
    }
    int ret = api_->SubscribeMarketData(instruments, arr.Length());
    for (size_t i = 0; i < arr.Length(); i++) {
      delete instruments[i];
    }
    delete instruments;
    return Napi::Number::New(info.Env(), ret);
  }

  Napi::Value UnSubscribeMarketData(const CallbackInfo &info) {
    auto arr = info[0].As<Napi::Array>();
    auto instruments = new char *[arr.Length()];
    for (size_t i = 0; i < arr.Length(); i++) {
      Napi::Value v = arr[i];
      std::string str = v.As<Napi::String>();
      instruments[i] = new char[str.size()];
      memcpy(instruments[i], str.c_str(), str.size());
    }
    int ret = api_->UnSubscribeMarketData(instruments, arr.Length());
    for (size_t i = 0; i < arr.Length(); i++) {
      delete instruments[i];
    }
    delete instruments;
    return Napi::Number::New(info.Env(), ret);
  }

  Napi::Value SubscribeForQuoteRsp(const CallbackInfo &info) {
    auto arr = info[0].As<Napi::Array>();
    auto instruments = new char *[arr.Length()];
    for (size_t i = 0; i < arr.Length(); i++) {
      Napi::Value v = arr[i];
      std::string str = v.As<Napi::String>();
      instruments[i] = new char[str.size()];
      memcpy(instruments[i], str.c_str(), str.size());
    }
    int ret = api_->SubscribeForQuoteRsp(instruments, arr.Length());
    for (size_t i = 0; i < arr.Length(); i++) {
      delete instruments[i];
    }
    delete instruments;
    return Napi::Number::New(info.Env(), ret);
  }

  Napi::Value UnSubscribeForQuoteRsp(const CallbackInfo &info) {
    auto arr = info[0].As<Napi::Array>();
    auto instruments = new char *[arr.Length()];
    for (size_t i = 0; i < arr.Length(); i++) {
      Napi::Value v = arr[i];
      std::string str = v.As<Napi::String>();
      instruments[i] = new char[str.size()];
      memcpy(instruments[i], str.c_str(), str.size());
    }
    int ret = api_->UnSubscribeForQuoteRsp(instruments, arr.Length());
    for (size_t i = 0; i < arr.Length(); i++) {
      delete instruments[i];
    }
    delete instruments;
    return Napi::Number::New(info.Env(), ret);
  }

  void ReqUserLogin(const CallbackInfo &info) {
    auto ab = info[0].As<Napi::TypedArray>().ArrayBuffer();
    int32_t nRequestID = info[1].As<Napi::Number>().Int32Value();
    CThostFtdcReqUserLoginField *req = (CThostFtdcReqUserLoginField *)ab.Data();
    api_->ReqUserLogin(req, nRequestID);
  }

  void ReqUserLogout(const CallbackInfo &info) {
    auto ab = info[0].As<Napi::TypedArray>().ArrayBuffer();
    int32_t nRequestID = info[1].As<Napi::Number>().Int32Value();
    CThostFtdcUserLogoutField *req = (CThostFtdcUserLogoutField *)ab.Data();
    api_->ReqUserLogout(req, nRequestID);
  }

  void OnFrontConnected() { callJS(new Message("OnFrontConnected", "", 0)); }

  void OnFrontDisconnected(int nReason) {
    callJS(new Message("OnFrontDisconnected", "", 0));
  }
  void OnHeartBeatWarning(int nTimeLapse) {
    callJS(new Message("OnHeartBeatWarning", "", 0));
  }
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) {
    callJS(make_message("OnRspUserLogin", pRspUserLogin, pRspInfo, nRequestID,
                        bIsLast));
  }
  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast) {
    callJS(make_message("OnRspUserLogout", pUserLogout, pRspInfo, nRequestID,
                        bIsLast));
  }
  void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                  bool bIsLast) {
    callJS(make_message("OnRspError", pRspInfo, pRspInfo, nRequestID, bIsLast));
  }
  void
  OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                     CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                     bool bIsLast) {
    callJS(make_message("OnRspSubMarketData", pSpecificInstrument, pRspInfo,
                        nRequestID, bIsLast));
  }
  void
  OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast) {
    callJS(make_message("OnRspUnSubMarketData", pSpecificInstrument, pRspInfo,
                        nRequestID, bIsLast));
  }
  void
  OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) {
    callJS(make_message("OnRspSubForQuoteRsp", pSpecificInstrument, pRspInfo,
                        nRequestID, bIsLast));
  }
  void
  OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast) {
    callJS(make_message("OnRspUnSubForQuoteRsp", pSpecificInstrument, pRspInfo,
                        nRequestID, bIsLast));
  }
  void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
    callJS(make_message("OnRtnDepthMarketData", pDepthMarketData));
  }
  void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {
    callJS(make_message("OnRtnForQuoteRsp", pForQuoteRsp));
  }
};

} // namespace addon