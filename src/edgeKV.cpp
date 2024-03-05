#include "edge/EdgeService.h"
#include "utill/utill.h"
#include <sstream>
int main(int argc, char const *argv[]) {
  // EdgeService service;
  // service.client->put("111", "222");
  // std::string value;
  // service.client->get("a", value);
  // service.client->del("111");
  // INFO(value);
  // INFO(service.client->get("111", value));
  S3FIFO cache;
  string command;
  string key;
  string value = "";
  while (true) {
    std::cin >> command;
    std::cin >> key;

    if (command == "get") {
      cache.get(key, value);
      INFO("get key:{} ,value:{}", key, value);
    } else if (command == "put") {
      std::cin >> value;
      cache.put(key, value);
      INFO("put key:{} ,value:{}", key, value);
    } else if (command == "del") {
      cache.del(key);
      INFO("del key:{}", key);
    } else {
      WARN("wrong command");
    }
    std::cin.ignore();
  }
  return 0;
}
