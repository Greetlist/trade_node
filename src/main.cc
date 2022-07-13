#include <iostream>
#include <unordered_map>
#include "trade_node.h"

using namespace std;

int main(int argc, char** argv)
{
  greetlist::trader::TradeNode* node = new greetlist::trader::TradeNode(std::string("node_config.yml"));
  node->Init();
  node->Start();
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
