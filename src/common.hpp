
#ifndef COMMON_XX_9921_
#define COMMON_XX_9921_

#include <napi.h>
#include <string>
#include <thread>
#include <vector>

struct CThostFtdcRspInfoField;
namespace addon {
struct Message {
public:
  const char *type_;   // 消息类型
  void *data_;         // 数据
  size_t data_len_;    // 数据长度
  char error_msg_[81]; // 错误信息
  int32_t error_id_;   // 错误编号
  int32_t request_id_; // nRequestID
  bool is_last_;       // bIsLast
  int argc_;           //
  Message(const char *t, void *d, size_t n)
      : type_(t), data_(nullptr), data_len_(n), argc_(2) {
    if (d) {
      data_ = malloc(n);
      memcpy(data_, d, n);
    }
  }
  Message(const char *t, void *d, size_t n, int32_t eid, const char *emsg,
          int32_t request_id, bool islast)
      : type_(t), data_(nullptr), data_len_(n), error_id_(eid),
        request_id_(request_id), is_last_(islast), argc_(6) {
    if (eid != 0 && emsg) {
      memcpy(error_msg_, emsg, sizeof(error_msg_));
    } else {
      error_msg_[0] = 0;
    }
    if (d) {
      data_ = malloc(n);
      memcpy(data_, d, n);
    }
  }
  ~Message() { free(data_); }
};

template <typename T1>
Message *make_message(const char *type, T1 *t1,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) {
  if (pRspInfo) {
    return new Message(type, t1, sizeof(T1), pRspInfo->ErrorID,
                       pRspInfo->ErrorMsg, nRequestID, bIsLast);
  }
  return new Message(type, t1, sizeof(T1), 0, nullptr, nRequestID, bIsLast);
}

template <typename T1>
Message *make_message(const char *type, T1 *t1,
                      CThostFtdcRspInfoField *pRspInfo) {
  if (pRspInfo) {
    return new Message(type, t1, sizeof(T1), pRspInfo->ErrorID,
                       pRspInfo->ErrorMsg, 0, true);
  }
  return new Message(type, t1, sizeof(T1), 0, nullptr, 0, true);
}

template <typename T1> Message *make_message(const char *type, T1 *t1) {
  return new Message(type, t1, sizeof(T1), 0, "", 0, true);
}

} // namespace addon
#endif