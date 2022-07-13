#include "tcp_server.h"
namespace greetlist::trader {

TcpServer::TcpServer(const int& port, EventBase* base) : base_(base), port_(port), listener_(nullptr) {
}

TcpServer::~TcpServer() {
  Stop();
}

bool TcpServer::Init() {
  if (!InitListener()) {
    LOG(ERROR) << "Init Server Error";
    return false;
  }
  return true;
}

void TcpServer::Start() {
  event_base_dispatch(base_);
}

void TcpServer::Stop() {
  event_base_loopexit(base_, nullptr);
  if (base_) {
    event_base_free(base_);
    base_ = nullptr;
  }
  if (listener_) {
    evconnlistener_free(listener_);
    listener_ = nullptr;
  }
}

bool TcpServer::InitListener() {
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port_);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  listener_ = evconnlistener_new_bind(base_, OnClientAccept, (void*)this, LEV_OPT_REUSEABLE | BEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&server, sizeof(server));
  if (!listener_) {
    LOG(ERROR) << "Create Listener Error";
    return false;
  }
  return true;
}

bool TcpServer::SetNonBlock(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int status = fcntl(fd, F_SETFL, flags);
  if (status < 0) {
  	perror("File Control Error");
    return false;
  }
  return true;
}

bool TcpServer::SetBufferSize(int fd) {
  socklen_t l = sizeof(socket_buffer_size_);
  int set_rev_status = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &socket_buffer_size_, l);
  int set_send_status = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &socket_buffer_size_, l);
  if (set_rev_status < 0 || set_send_status < 0) {
    LOG(ERROR) << "setsockopt error";
    return false;
  }
  return true;
}

void TcpServer::BindCallBack(ConnectSuccessCallBack conn_cb, DisconnectCallBack disconn_cb, RecvDataCallBack recv_cb) {
  conn_callback_ = conn_cb;
  disconn_callback_ = disconn_cb;
  recv_data_callback_ = recv_cb;
}

static void OnClientAccept(EvConnListener* listener, int client_fd, struct sockaddr* addr, int socklen, void* user_arg) {
  evutil_make_socket_nonblocking(client_fd);
  TcpServer* server = reinterpret_cast<TcpServer*>(user_arg);
  EventBuffer* ev_buff = bufferevent_socket_new(server->base_, client_fd, 0);
  bufferevent_setcb(ev_buff, OnReadFromClient, OnWriteToClient, OnClientEvent, user_arg);
  bufferevent_enable(ev_buff, EV_READ|EV_WRITE);
  server->conn_callback_(ev_buff, client_fd);
}

static void OnReadFromClient(EventBuffer* bev, void* ctx) {
  TcpServer* server = reinterpret_cast<TcpServer*>(ctx);
  std::shared_ptr<char> data = std::shared_ptr<char>(new char[TcpServer::read_buffer_size_]);
  std::size_t n_read;
  while (true) {
    n_read = bufferevent_read(bev, data.get(), TcpServer::read_buffer_size_);
    if (n_read <= 0) {
      break;
    }
  }
  int client_fd = bufferevent_getfd(bev);
  server->recv_data_callback_(client_fd, data);
}

static void OnWriteToClient(EventBuffer* bev, void* ctx) {
  TcpServer* server = reinterpret_cast<TcpServer*>(ctx);
}

static void OnClientEvent(EventBuffer* bev, short events, void* ctx) {
  TcpServer* server = reinterpret_cast<TcpServer*>(ctx);
  DisConnectType t;
  struct evbuffer* input = bufferevent_get_input(bev);
  int finished = 0;
  if (events & BEV_EVENT_EOF) {
    LOG(INFO) << "Client Close Connection";
    finished = 1;
    t = DisConnectType::CLOSE;
  }
  if (events & BEV_EVENT_ERROR) {
    LOG(ERROR) << evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
    finished = 1;
    t = DisConnectType::ERROR;
  }
  if (finished) {
    server->disconn_callback_(t);
  }
}

} //namespace greetlist::trader
