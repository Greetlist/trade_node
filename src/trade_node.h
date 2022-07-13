#ifndef TRADE_NODE_H_
#define TRADE_NODE_H_

#include <unordered_map>
#include <thread>

#include "tcp_session_manager.h"

namespace greetlist::trader {
class TradeNode {
public:
  explicit TradeNode(const std::string&& config);
  TradeNode operator=(const TradeNode& node) = delete;
  ~TradeNode();
  void Init();
  void Start();
private:
  TcpSessionManager* session_manager_;
  std::string config_file_;
  std::thread session_thread_;
};

} //namespace greetlist::trader

#endif 
