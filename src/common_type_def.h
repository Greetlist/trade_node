#ifndef COMMON_TYPE_DEF_H_
#define COMMON_TYPE_DEF_H_

#include "event.h"
#include <functional>
#include <memory>

namespace greetlist::trader {

enum class DisConnectType {
  CLOSE = 1,
  ERROR = 2
};

typedef struct event_base EventBase;
typedef struct event_config EventConfig;
typedef struct evconnlistener EvConnListener;
typedef struct bufferevent EventBuffer;
typedef std::function<void(EventBuffer*, int)> ConnectSuccessCallBack;
typedef std::function<void(DisConnectType)> DisconnectCallBack;
typedef std::function<void(int, std::shared_ptr<char>)> RecvDataCallBack;

} //namespace greetlist::trader

#endif
