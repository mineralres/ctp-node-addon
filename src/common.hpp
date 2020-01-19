
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
  const char *type_;
  std::string data_;
  char error_msg_[81];
  int32_t error_id_;
  int32_t request_id_;
  bool is_last_;
  int argc_;
  Message(const char *t, void *d, size_t n)
      : type_(t), data_((const char *)d, n), argc_(2) {}
  Message(const char *t, void *d, size_t n, int32_t eid, const char *emsg,
          int32_t request_id, bool islast)
      : type_(t), data_((const char *)d, n), error_id_(eid),
        request_id_(request_id), is_last_(islast), argc_(6) {
    if (eid != 0) {
      memcpy(error_msg_, emsg, sizeof(error_msg_));
    } else {
      error_msg_[0] = 0;
    }
  }
};

template <typename T1>
Message *make_message(const char *type, T1 *t1,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast) {
  return new Message(type, t1, sizeof(T1), pRspInfo->ErrorID,
                     pRspInfo->ErrorMsg, nRequestID, bIsLast);
}

template <typename T1>
Message *make_message(const char *type, T1 *t1,
                      CThostFtdcRspInfoField *pRspInfo) {
  return new Message(type, t1, sizeof(T1), pRspInfo->ErrorID,
                     pRspInfo->ErrorMsg, 0, true);
}

template <typename T1> Message *make_message(const char *type, T1 *t1) {
  return new Message(type, t1, sizeof(T1), 0, "", 0, true);
}

} // namespace addon
#endif