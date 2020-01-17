// hello.cc
#include "./md.hpp"
#include "./trade.hpp"
#include <node.h>
#include <v8.h>


namespace addon {
using namespace v8;

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Object;
using v8::String;
using v8::Value;

void Initialize(Local<Object> exports) {
  NODE_SET_METHOD(exports, "CreateMdApi", CreateMdApi);
  NODE_SET_METHOD(exports, "MdApiCall", MdApiCall);
  NODE_SET_METHOD(exports, "ReleaseMdApi", ReleaseMdApi);
  NODE_SET_METHOD(exports, "CreateTraderApi", CreateTraderApi);
  NODE_SET_METHOD(exports, "TraderApiCall", TraderApiCall);
  NODE_SET_METHOD(exports, "ReleaseTraderApi", ReleaseTraderApi);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

} // namespace addon