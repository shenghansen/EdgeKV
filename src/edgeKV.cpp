#include "edge/EdgeService.h"
#include "utill/utill.h"
#include <sstream>
// DEFINE_bool(KV_attachment, true, "Echo attachment as well");
DEFINE_int32(port, 8000, "TCP Port of this server");
DEFINE_string(listen_addr, "",
              "Server listen address, may be IPV4/IPV6/UDS."
              " If this is set, the flag port will be ignored");
DEFINE_int32(idle_timeout_s, -1,
             "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");

int main(int argc, char *argv[]) {

  // Parse gflags. We recommend you to use gflags as well.
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // Generally you only need one Server.
  brpc::Server server;

  S3FIFO cache;
  UpdateClientServiceImpl updateClientService(&cache);
  HttpServiceImpl httpService(&cache);
  if (server.AddService(&updateClientService,
                        brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
    LOG(ERROR) << "Fail to add listen_datacenter_service";
    return -1;
  }
  if (server.AddService(&httpService, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
    LOG(ERROR) << "Fail to add httpservice";
    return -1;
  }

  butil::EndPoint point;
  if (!FLAGS_listen_addr.empty()) {
    if (butil::str2endpoint(FLAGS_listen_addr.c_str(), &point) < 0) {
      LOG(ERROR) << "Invalid listen address:" << FLAGS_listen_addr;
      return -1;
    }
  } else {
    point = butil::EndPoint(butil::IP_ANY, FLAGS_port);
  }
  brpc::ServerOptions options;
  options.idle_timeout_sec = FLAGS_idle_timeout_s;
  if (server.Start(point, &options) != 0) {
    LOG(ERROR) << "Fail to start HttpServer and UpdateClientServer";
    return -1;
  }
  server.RunUntilAskedToQuit();
  return 0;

  // EdgeService service;
  // service.client->put("111", "222");
  // std::string value;
  // service.client->get("a", value);
  // service.client->del("111");
  // INFO(value);
  // INFO(service.client->get("111", value));

  // S3FIFO cache;
  // string command;
  // string key;
  // string value = "";
  // while (true) {
  //   std::cin >> command;
  //   std::cin >> key;

  //   if (command == "get") {
  //     cache.get(key, value);
  //     INFO("get key:{} ,value:{}", key, value);
  //   } else if (command == "put") {
  //     std::cin >> value;
  //     cache.put(key, value);
  //     INFO("put key:{} ,value:{}", key, value);
  //   } else if (command == "del") {
  //     cache.del(key);
  //     INFO("del key:{}", key);
  //   } else {
  //     WARN("wrong command");
  //   }
  //   std::cin.ignore();
  // }
  // return 0;
}
