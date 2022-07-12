#ifndef TCP_SESSION_MANAGER_H_
#define TCP_SESSION_MANAGER_H_

#include <unordered_map>

#include "tcp_server.h"
#include "tcp_session.h"
#include "common_type_def.h"

namespace greetlist::trader {

class TcpSessionManager {
public:
  TcpSessionManager(std::unordered_map<std::string, std::string>&& config);
  TcpSessionManager operator=(const TcpSessionManager& m) = delete;
  ~TcpSessionManager();
  void Init();
  bool InitServer();
  bool GetDownstreamInfo();
  bool InitDownstream();
  TcpSession* InitSingleSession(const std::string& host, const int& port);
  void Start();
  void Stop();
  EventBase* GetEventBase();
  void OnConnect(EventBuffer* ev_buff, int client_fd);
  void OnDisConnect(DisConnectType disconn_type);
  void OnRecvData(std::shared_ptr<char> data);
private:
  int port_ = -1;
  TcpServer* tcp_server_;
  EventBase* base_;
  std::unordered_map<std::string, std::string> config_;
  std::unordered_map<std::string, std::pair<std::string, int>> downstream_info_;
  //std::unordered_map<int, std::shared_ptr<TcpSession>> downstream_;
  //std::unordered_map<int, std::shared_ptr<TcpSession>> upstream_;
  std::vector<std::shared_ptr<TcpSession>> upstream_;
  std::vector<std::shared_ptr<TcpSession>> downstream_;
};

} //namespace greetlist::trader

#endif
