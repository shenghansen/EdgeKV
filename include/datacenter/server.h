#include "utill/KVService.pb.h"
#include "utill/leveldb.h"
#include "utill/utill.h"
#include <brpc/server.h>
#include <json2pb/pb_to_json.h>
#include <string>

class KVServiceImpl : public KVService {
public:
  KVServiceImpl();
  virtual ~KVServiceImpl();
  virtual void Get(google::protobuf::RpcController *cntl_base,
                   const GetRequest *request, GetResponse *response,
                   google::protobuf::Closure *done);
  virtual void Put(google::protobuf::RpcController *cntl_base,
                   const PutRequest *request, PutResponse *response,
                   google::protobuf::Closure *done);
  virtual void Del(google::protobuf::RpcController *cntl_base,
                   const DelRequest *request, DelResponse *response,
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

private:
  Level_db *db;
};
