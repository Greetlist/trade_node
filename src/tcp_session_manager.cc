#include "tcp_server_manager.h"

namespace greetlist::trader {

TcpSessionManager::TcpSessionManager(std::unordered_map<std::string, std::string>&& config): config_(config), tcp_server_(nullptr) {
  port_ = std::stoi(config["server_port"]);
}

TcpSessionManager::~TcpSessionManager() {
  delete tcp_server_;
}

void TcpSessionManager::Init() {
  base_ = GetEventBase();
  CHECK(InitServer());
  CHECK(GetDownstreamInfo(config["downstream_file"]));
  InitDownstream();
}

bool TcpSessionManager::InitServer() {
  tcp_server_ = new TcpServer(port_, server_base);
  tcp_server_.BindCallBack(OnConnect, OnDisConnect, OnRecvData);
  return tcp_server_->Init();
}

bool TcpSessionManager::GetDownstreamInfo(const std::string& config_file) {
  YAML::Node downstream_config = YAML::LoadFile(config_file);
  YAML::Node client_seq = downstream_config["down_stream"];
  CHECK(client_seq.Type() == YAML::NodeType::Sequence);
  for (const auto& client_info : client_seq) {
    downstream_info_[client_info["node_id"]] = std::make_pair(client_info["host"], std::stoi(client_info["port"]));
  }
}

bool TcpSessionManager::InitDownstream() {
  for (const auto& down_node : down_stream_info_) {
    const auto& [host, port] = connect_info;
    TcpSession* session = InitSingleSession(host, port);
    if (session) {
      //TODO
      // send router info and update route table.
    }
  }
}

TcpSession* TcpSessionManager::InitSingleSession(const std::string& host, const int& port) {
  int fd = socket(AF_ITEN, SOCK_STREAM, 0);
  if (fd < 0) {
    LOG(ERROR) << "Create Socket Error";
    return nullptr;
  }
  evutil_make_socket_nonblocking(fd);
  EventBuffer* buffer = bufferevent_socket_new(base_, fd, 0);
  if (!ev_buff) {
    LOG(ERROR) << "Create EvBuff Error";
    evbuffer_free(buffer);
    return false;
  }
  bufferevent_setcb(buffer, OnRead, OnWrite, OnEvent);
  bufferevent_enable(buffer, EV_READ|EV_WRITE);

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  inet_aton(host.c_str(), &server_addr.sin_addr);
  server_addr.sin_port = htons(port);
  int res = bufferevent_socket_connect(buffer, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (res != 0) {
    LOG(ERROR) << "Failed Connect to " << host << ":" << port;
    evbuffer_free(buffer);
    return nullptr;
  }
  return new TcpSession(buffer, fd);
}

void TcpSessionManager::Start() {
  event_base_dispatch(base_);
}

void TcpSessionManager::Stop() {
  tcp_server_->Stop();
}

EventBase* TcpSessionManager::GetEventBase() {
  EventConfig* cfg = event_config_new();
  if (!cfg) {
    return false;
  }
  event_config_avoid_method(cfg, "select");
  event_config_avoid_method(cfg, "poll");
  event_config_require_features(cfg, EV_FEATURE_ET);
  EventBase* base = event_base_new_with_config(cfg);
  event_config_free(cfg);
  if (!base) {
    return nullptr;
  }
  return base;
}

void TcpSessionManager::OnConnect(EventBuffer* ev_buff, int client_fd) {
  TcpSession* upstream_session = new TcpSession(ev_buff, client_fd);
  //upstream_.emplace_back(std::make_shared<TcpSession>(upstream_session));
  upstream_.emplace_back(upstream_session);
  // update router table and add session
}

void TcpSessionManager::OnDisConnect(DisConnectType disconn_type) {
}

void OnRecvData(int client_fd, std::shared_ptr<char> data) {
  LOG(INFO) << "Recv data: " << data.get();
}

} //namespace greetlist::trader
