// addon.cc
#include "./md.hpp"
#include "./trader.hpp"
#include <assert.h>
#include <napi.h>

namespace addon {

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports = TraderApi::Init(env, exports);
  exports = MdApi::Init(env, exports);
  return exports;
}

NODE_API_MODULE(addon, Init)

} // namespace addon