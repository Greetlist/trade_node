#ifndef TCP_SESSION_MANAGER_H_
#define TCP_SESSION_MANAGER_H_

#include <unordered_map>

#include "tcp_server.h"
#include "tcp_session.h"
#include "common_type_def.h"
#include "yaml-cpp/yaml.h"

namespace greetlist::trader {

class TcpSessionManager {
public:
  TcpSessionManager(const std::string& config_file);
  TcpSessionManager operator=(const TcpSessionManager& m) = delete;
  ~TcpSessionManager();
  void Init();
  bool InitServer();
  void InitConfigFile();
  void InitDownstream();
  TcpSession* InitSingleSession(const std::string& host, const int& port);
  void SetBufferEventCallBack(EventBuffer* buffer);
  void Start();
  void Stop();
  EventBase* GetEventBase();
private:
  int server_port_ = -1;
  TcpServer* tcp_server_;
  EventBase* base_;
  std::string config_file_;
  std::unordered_map<std::string, std::string> config_;
  std::unordered_map<std::string, std::pair<std::string, int>> downstream_info_;
  //std::unordered_map<int, std::shared_ptr<TcpSession>> downstream_;
  //std::unordered_map<int, std::shared_ptr<TcpSession>> upstream_;
  std::vector<std::shared_ptr<TcpSession>> upstream_;
  std::vector<std::shared_ptr<TcpSession>> downstream_;
};

} //namespace greetlist::trader

#endif
