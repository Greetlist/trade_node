#include "tcp_session_manager.h"

namespace greetlist::trader {

TcpSessionManager::TcpSessionManager(const std::string& config_file): config_file_(config_file), tcp_server_(nullptr) {
  InitConfigFile();
}

TcpSessionManager::~TcpSessionManager() {
  delete tcp_server_;
}

void TcpSessionManager::Init() {
  base_ = GetEventBase();
  CHECK(InitServer());
  InitDownstream();
}

bool TcpSessionManager::InitServer() {
  tcp_server_ = new TcpServer(server_port_, base_);
  ConnectSuccessCallBack conn_callback = [this](EventBuffer* buffer, int fd) {
    LOG(INFO) << "OnConnect";
  };
  DisconnectCallBack disconn_callback = [this](DisConnectType disconn_type) {
    LOG(INFO) << "OnDisconnect";
  };
  RecvDataCallBack recv_callback = [this](int fd, std::shared_ptr<char> data) {
    LOG(INFO) << "OnRecv";
  };

  tcp_server_->BindCallBack(conn_callback, disconn_callback, recv_callback);
  return tcp_server_->Init();
}

void TcpSessionManager::InitConfigFile() {
  YAML::Node config = YAML::LoadFile(config_file_);
  server_port_ = config["server_port"].as<int>();
  YAML::Node client_seq = config["down_stream"];
  CHECK(client_seq.Type() == YAML::NodeType::Sequence);
  for (const auto& client_info : client_seq) {
    std::string node_id = client_info["node_id"].as<std::string>();
    std::string host = client_info["host"].as<std::string>();
    int port = std::stoi(client_info["port"].as<std::string>());
    downstream_info_[node_id] = std::make_pair(host, port);
  }
}

void TcpSessionManager::InitDownstream() {
  for (const auto& [node_id, down_node] : downstream_info_) {
    const auto& [host, port] = down_node;
    TcpSession* session = InitSingleSession(host, port);
    if (session) {
      LOG(INFO) << "Connect Success, host:" << host << ", port: " << port;
      //TODO
      // send router info and update route table.
    } else {
      LOG(INFO) << "Init Session Error";
    }
  }
}

TcpSession* TcpSessionManager::InitSingleSession(const std::string& host, const int& port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    LOG(ERROR) << "Create Socket Error";
    return nullptr;
  }
  EventBuffer* buffer = bufferevent_socket_new(base_, fd, 0);
  if (!buffer) {
    LOG(ERROR) << "Create EvBuff Error";
    bufferevent_free(buffer);
    return nullptr;
  }
  SetBufferEventCallBack(buffer);
  bufferevent_enable(buffer, EV_READ|EV_WRITE);

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  inet_aton(host.c_str(), &server_addr.sin_addr);
  server_addr.sin_port = htons(port);
  int res = bufferevent_socket_connect(buffer, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (res != 0) {
    LOG(ERROR) << "Failed Connect to " << host << ":" << port;
    bufferevent_free(buffer);
    return nullptr;
  }
  evutil_make_socket_nonblocking(fd);
  return new TcpSession(buffer, fd);
}

void TcpSessionManager::SetBufferEventCallBack(EventBuffer* buffer) {
  //auto read_fn = [](EventBuffer* bev, void* ctx) {
  //  char buf[4096];
  //  while (true) {
  //    if (int n_read = bufferevent_read(bev, buf, 4096); n_read <= 0) {
  //      break;
  //    }
  //  }
  //  LOG(INFO) << "Recv from Server data: " << buf;
  //};

  //bufferevent_data_cb ff = reinterpret_cast<bufferevent_data_cb>(read_fn);

  //OnWriteCallBack write_fn = [](EventBuffer* bev, void* ctx) {
  //  LOG(INFO) << "Send to Server";
  //};

  //OnEventCallBack event_fn = [](EventBuffer* bev, short events, void* ctx) {
  //  LOG(INFO) << "On Event";
  //};
  bufferevent_setcb(
    buffer,
    [](EventBuffer* bev, void* ctx) {LOG(INFO) << "On Read";},
    [](EventBuffer* bev, void* ctx) {LOG(INFO) << "On Write";},
    [](EventBuffer* bev, short events, void* ctx) {
      if (events & BEV_EVENT_ERROR) {
        LOG(ERROR) << evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
      }
    },
    this
  );
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
    event_config_free(cfg);
    return nullptr;
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

} //namespace greetlist::trader
