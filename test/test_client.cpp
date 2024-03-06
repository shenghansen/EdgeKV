#include "edge/client.h"
#include "utill/utill.h"
#include <string>
using std::string;

int main(int argc, char *argv[]) {
  string *key_list = new string[PREFETCH_RANGE];
  string *value_list = new string[PREFETCH_RANGE];
  bool exist;
  int num;
  KVClient client;
  //   client.put("a", "b");
  //   client.put("b", "b");
  //   client.put("c", "b");
  //   client.put("d", "b");
  //   client.put("e", "b");
  //   client.put("f", "b");
  //   client.put("g", "b");
  //   client.put("h", "b");
  //   client.put("aaa", "b");
  //   client.put("aa", "b");
  client.getRange(exist, num, "a", key_list, value_list);
}