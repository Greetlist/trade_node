#include "trade_node.h"

namespace greetlist::trader {

TradeNode::TradeNode(const std::unordered_map<std::string, std::string>&& config) : config_(config) {
}

void TradeNode::Init() {
  session_manager_ = new TcpSessionManager(config_);
}

void TradeNode::Start() {
  session_thread_ = std::thread(
    []() {
    session_manager->Init();
    session_manger->Start();
  });
}

} //namespace greetlist::trader
