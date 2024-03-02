#include "edge/EdgeService.h"
#include "utill/utill.h"

int main(int argc, char const *argv[]) {
  EdgeService service;
  service.client->put("111", "222");
  std::string value;
  service.client->get("a", value);
  service.client->del("111");
  INFO(value);
  INFO(service.client->get("111", value));

  return 0;
}
