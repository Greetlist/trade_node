#include "tcp_session.h"

namespace greetlist::trader {
TcpSession::TcpSession(EventBuffer* ev_buff, int fd) : ev_buff_(ev_buff), fd_(fd) {}

TcpSession::~TcpSession() {
  if (buffer) {
    bufferevent_free(buffer);
    buffer = nullptr;
  }
}

void TcpSession::Send(void* msg, std::size_t len) {
  int res = bufferevent_write(ev_buff_, msg, len);
  if (res != 0) {
    LOG(ERROR) << "Write to event buffer error";
  }
}

} //namespace greetlist::trader
