#ifndef TCP_CLIENTS_MANAGER_H_
#define TCP_CLIENTS_MANAGER_H_

#include <unordered_map>
#include <memory>
#include <assert>
#include "yaml.h"

namespace greetlist::trader {

class TcpClientsManager {
public:
  explicit TcpClientsManager(const std::string& config_file);
  ~TcpClientsManager();
  bool GetDownstreamInfo();
  bool InitAllDownstream();
  void Start();
  void InitEventBase();
private:
  EventBase* client_base_;
};

} //namespace greetlist::trader

#endif
