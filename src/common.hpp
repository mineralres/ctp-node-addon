#ifndef COMMON_XX_9921_
#define COMMON_XX_9921_
#include <node.h>
#include <string>
#include <thread>
#include <uv.h>
#include <v8.h>
#include <vector>

namespace addon {
struct Message {
public:
  const char *type_;
  std::string data_;
  char error_msg_[81];
  int32_t error_id_;
  int32_t request_id_;
  bool is_last_;
  int argc_;
  Message(const char *t, const char *d, size_t n)
      : type_(t), data_(d, n), argc_(2) {}
  Message(const char *t, const char *d, size_t n, int32_t eid, const char *emsg,
          int32_t request_id, bool islast)
      : type_(t), data_(d, n), error_id_(eid), request_id_(request_id),
        is_last_(islast), argc_(6) {
    if (eid != 0) {
      memcpy(error_msg_, emsg, sizeof(error_msg_));
    } else {
      error_msg_[0] = 0;
    }
  }
};

template <typename T> void onCtpMessage(uv_async_t *handle) {
  Isolate *isolate = Isolate::GetCurrent();
  v8::HandleScope scope(isolate);
  T *data = (T *)handle->data;
  std::lock_guard<std::mutex> lock(data->mut_);
  for (int i = 0; i < data->msg_list_.size(); i++) {
    auto msg = data->msg_list_[i];
    if (msg->argc_ == 2) {
      v8::Local<v8::ArrayBuffer> ab =
          v8::ArrayBuffer::New(isolate, msg->data_.size());
      memcpy(ab->GetContents().Data(), msg->data_.c_str(), msg->data_.size());
      v8::Local<v8::Value> argv[] = {
          v8::String::NewFromUtf8(isolate, msg->type_).ToLocalChecked(), ab};
      Local<Context> context = isolate->GetCurrentContext();
      Local<Function>::New(isolate, data->callback_)
          ->Call(context, Null(isolate), 2, argv);
    } else if (msg->argc_ == 6) {
      v8::Local<v8::ArrayBuffer> ab =
          v8::ArrayBuffer::New(isolate, msg->data_.size());
      memcpy(ab->GetContents().Data(), msg->data_.c_str(), msg->data_.size());
      v8::Local<v8::Uint8Array> errorMsg =
          Local<v8::Uint8Array>::Cast(v8::ArrayBuffer::New(
              isolate, msg->error_msg_, sizeof(msg->error_msg_)));
      v8::Local<v8::Value> argv[] = {
          v8::String::NewFromUtf8(isolate, msg->type_).ToLocalChecked(),
          ab,
          v8::Integer::New(isolate, msg->error_id_),
          errorMsg,
          v8::Integer::New(isolate, msg->request_id_),
          v8::Boolean::New(isolate, msg->is_last_)};
      Local<Context> context = isolate->GetCurrentContext();
      Local<Function>::New(isolate, data->callback_)
          ->Call(context, Null(isolate), 6, argv);
    }
    delete msg;
  }
  data->msg_list_.clear();
}

} // namespace addon
#endif