#include "tcp_clients_manager.h"

namespace greetlist::trader {

TcpClientsManager::TcpClientsManager(const std::string& config_file) {
  CHECK(GetDownstreamInfo(config_file));
}

TcpClientsManager::~TcpClientsManager() {
}

void TcpClientsManager::InitEventBase() {
  EventConfig* cfg = event_config_new();
  if (!cfg) {
    return false;
  }
  event_config_avoid_method(cfg, "select");
  event_config_avoid_method(cfg, "poll");
  event_config_require_features(cfg, EV_FEATURE_ET);
  client_base_ = event_base_new_with_config(cfg);
  event_config_free(cfg);
  CHECK(client_base_);
}

bool GetDownstreamInfo(const std::string& config_file) {
  YAML::Node config = YAML::LoadFile(config_file);
  YAML::Node client_seq = config["down_stream"];
  CHECK(client_seq.Type() == YAML::NodeType::Sequence);
  for (const auto& client_info : client_seq) {
    downstream_info_[client_info["node_id"]] = std::make_pair(client_info["host"], stoi(client_info["port"]));
  }
}

bool TcpClientsManager::InitAllDownstream() {
  InitEventBase();
  for (const auto& down_node : down_stream_info_) {
    const auto& [host, port] = connect_info;
    TcpClient* client = new TcpClient(client_base_, host, port);
    client->Init();
    if (client->Connect()) {
      std::shared_ptr<TcpSession> session(new TcpSession(client->GetFd(), client->GetBuffer()));
    }
  }
}

void TcpClientsManager::Start() {
  CHECK(InitAllDownstream());
  event_base_dispatch(base);
}

} //namespace greetlist::trader
