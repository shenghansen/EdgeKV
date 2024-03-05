#include "S3FIFO.h"
#include "utill/UpdateClient.pb.h"
#include "utill/leveldb.h"
#include <brpc/server.h>
#include <json2pb/pb_to_json.h>
#include <thread>

class UpdateClientServiceImpl : public UpdateClientService {
public:
  // UpdateClientServiceImpl();
  // virtual ~UpdateClientServiceImpl();
  virtual void Update(google::protobuf::RpcController *cntl_base,
                      const GetRequest *request, GetResponse *response,
                      google::protobuf::Closure *done);

  // optional
  static void CallAfterRpc(brpc::Controller *cntl,
                           const google::protobuf::Message *req,
                           const google::protobuf::Message *res) {
    // at this time res is already sent to client, but cntl/req/res is not
    // destructed
    std::string req_str;
    std::string res_str;
    json2pb::ProtoMessageToJson(*req, &req_str, NULL);
    json2pb::ProtoMessageToJson(*res, &res_str, NULL);
    LOG(INFO) << "req:" << req_str << " res:" << res_str;
  }
};

class EdgeService {
public:
  EdgeService(const std::string &server = "0.0.0.0:8000",
              const std::string &load_balancer = "",
              const std::string &protocol = "baidu_std",
              const std::string &connection_type = "", int timeout_ms = 10000,
              int max_retry = 3, unsigned long cache_size = 1ULL << 10);
  ~EdgeService();
  void run();

private:
  brpc::Server listen_datacenter_server;
  UpdateClientServiceImpl *updateClientService;
  S3FIFO cache;
  // TODO httpserver

  // unused
  // KVClient *client;
  // Level_db *db;
  // std::unordered_set<std::string> uset;
};