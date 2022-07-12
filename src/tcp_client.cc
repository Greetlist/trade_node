#include "tcp_client.h"
namespace greetlist::trader {
TcpClient::TcpClient(EventBase* base, const std::string& addr, const int& port) : base_(base), end_point_addr_(addr), end_point_port_(port), fd_(-1) {
}

TcpClient::~TcpClient() {
  DisConnect();
}

bool TcpClient::Init() {
  CHECK(CreateSocketAndBuffer());
}

bool TcpClient::Connect() {
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  inet_aton(end_point_addr_.c_str(), &server_addr.sin_addr);
  server_addr.sin_port = htons(end_point_port_);
  int res = bufferevent_socket_connect(ev_buff_, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (res != 0) {
    LOG(ERROR) << "Failed Connect to " << end_point_addr << ":" << end_point_port_;
    return false;
  }
  return true;
}

TcpClient::DisConnect() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
  if (ev_buff_) {
    bufferevent_free(ev_buff_);
    ev_buff_ = nullptr;
  }
}

int TcpClient::GetFd() {
  return fd_;
}

EventBuffer* TcpClient::GetBuffer() {
  return ev_buff_;
}

bool TcpClient::Send(void* msg, std::size_t len) {
  int res = bufferevent_write(ev_buff_, msg, len);
  if (res != 0) {
    LOG(ERROR) << "Send Msg Error";
    return false;
  }
  return true;
}

bool TcpClient::CreateSocketAndBuffer() {
}

static void OnRead(EventBuffer* bev, void* ctx) {
}

static void OnWrite(EventBuffer* bev, void* ctx) {
}

static void OnEvent(EventBuffer* bev, short events, void* ctx) {
}

} //namespace greetlist::trader
