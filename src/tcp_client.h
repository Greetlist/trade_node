#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "glog/logging.h"
#include "event.h"
#include "evutil.h"
#include "common_type_def.h"

namespace greetlist::trader {

class TcpClient {
public:
  explicit TcpClient(EventBase* base, const std::string& addr, const int& port);
  TcpClient operator=(const TcpClient& client) = delete;
  ~TcpClient();
  bool Init();
  bool Connect();
  bool DisConnect();
  inline int GetFd();
  inline EventBuffer* GetBuffer();
  void Send(void* msg, std::size_t len);
private:
  int CreateSocketAndBuffer();
  std::string end_point_addr_;
  int end_point_port_;
  int fd_;
  EventBuffer* ev_buff_;
  EventBase* base_;
};

// c style callback function
static void OnRead(EventBuffer* bev, void* ctx);
static void OnWrite(EventBuffer* bev, void* ctx);
static void OnEvent(EventBuffer* bev, short events, void* ctx);

} //namespace greetlist::trader

#endif
