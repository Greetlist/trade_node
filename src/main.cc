#include <iostream>
#include <unordered_map>
#include "trade_node.h"

using namespace std;

int main(int argc, char** argv)
{
  std::unordered_map<std::string, std::string> config = {
    {"server_port", "3000"},
    {"downstream_file", "downstream.yml"}
  }
  TradeNode* node = new TradeNode(config);
  node->Init();
  node->Start();
  while (true) {
    std::this_thread::sleep_for(std::chrono::system_time::second(1));
  }
  return 0;
}
