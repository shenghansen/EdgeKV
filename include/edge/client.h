// #include "utill/HttpService.pb.h"
#include "utill/KVService.pb.h"
#include "utill/UpdateClient.pb.h"
#include "utill/utill.h"
#include <brpc/channel.h>
#include <string>

class KVClient {
public:
  KVClient(const std::string &server = "0.0.0.0:18081",
           const std::string &load_balancer = "",
           const std::string &protocol = "baidu_std",
           const std::string &connection_type = "", int timeout_ms = 10000,
           int max_retry = 3);
  ~KVClient();
  bool put(const std::string &key, const std::string &value);
  bool get(bool &exist, const std::string &key, std::string &value);
  bool getRange(bool &exist, int &num, const std::string &key,
                std::string *key_list, std::string *value_list);
  bool del(const std::string &key);

private:
  brpc::Channel channel;
  KVService_Stub *stub;
  brpc::ChannelOptions options;
};