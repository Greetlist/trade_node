#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <memory>
#include <unordered_map>
#include <functional>

#include "glog/logging.h"
#include "event.h"
#include "event2/listener.h"
#include "evutil.h"
#include "common_type_def.h"

namespace greetlist::trader {

class TcpServer {
public:
  explicit TcpServer(const int& port, const EventBase* base);
  ~TcpServer();
  TcpServer operator=(const TcpServer& src) = delete;
  bool Init();
  void Start();
  void Stop();
  bool InitListener();
  bool SetNonBlock(int fd);
  bool SetBufferSize(int fd);
  void BindCallBack(ConnectSuccessCallBack, DisconnectCallBack, RecvDataCallBack);
  static constexpr std::size_t socket_buffer_size_ = 1024 * 1024 * 10;
  static constexpr std::size_t read_buffer_size_ = 1024 * 1024 * 10;
  EventBase* base_;
private:
  int port_;
  EvConnListener* listener_;
  ConnectSuccessCallBack conn_callback_;
  DisconnectCallBack disconn_callback_;
  RecvDataCallBack recv_data_callback_;
};

// c style callback function
static void OnClientAccept(EvConnListener*, int, struct sockaddr*, int, void*);
static void OnReadFromClient(EventBuffer* bev, void* ctx);
static void OnWriteToClient(EventBuffer* bev, void* ctx);
static void OnClientEvent(EventBuffer* bev, short events, void* ctx);

} //namespace greetlist::trader

#endif
