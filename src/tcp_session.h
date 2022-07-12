#ifndef TCP_SESSION_H_
#define TCP_SESSION_H_

#include <string>
#include "glog/logging.h"
#include "tcp_server.h"
#include "common_type_def.h"

namespace greetlist::trader {

struct TcpSession {
public:
  explicit TcpSession(EventBuffer* ev_buff, int fd);
  ~TcpSession();
  void Send(void* msg, std::size_t len);

  int fd_;
  EventBuffer* buffer;
};

} //namespace greetlist::trader

#endif
