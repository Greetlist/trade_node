#include "trade_node.h"

namespace greetlist::trader {

TradeNode::TradeNode(const std::string&& config_file) : config_file_(config_file) {
}

void TradeNode::Init() {
  session_manager_ = new TcpSessionManager(config_file_);
}

void TradeNode::Start() {
  session_thread_ = std::thread(
    [this]() {
    session_manager_->Init();
    session_manager_->Start();
  });
}

} //namespace greetlist::trader
