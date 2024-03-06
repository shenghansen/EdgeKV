#include "S3FIFO.h"
#include "utill/HttpService.pb.h"
#include "utill/UpdateClient.pb.h"
#include "utill/leveldb.h"
#include <brpc/server.h>
#include <json2pb/pb_to_json.h>
#include <thread>

class UpdateClientServiceImpl : public UpdateClientService {
public:
  UpdateClientServiceImpl(S3FIFO *cache);
  // virtual ~UpdateClientServiceImpl();
  virtual void Update(google::protobuf::RpcController *cntl_base,
                      const UpdateClientRequest *request,
                      UpdateClientResponse *response,
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
  S3FIFO *cache;
};

class HttpServiceImpl : public HttpService {
public:
  HttpServiceImpl(S3FIFO *cache);
  virtual void Get(google::protobuf::RpcController *cntl_base,
                   const HttpRequest * /*request*/, HttpResponse * /*response*/,
                   google::protobuf::Closure *done);
  virtual void Put(google::protobuf::RpcController *cntl_base,
                   const HttpRequest * /*request*/, HttpResponse * /*response*/,
                   google::protobuf::Closure *done);
  virtual void Del(google::protobuf::RpcController *cntl_base,
                   const HttpRequest * /*request*/, HttpResponse * /*response*/,
                   google::protobuf::Closure *done);

private:
  S3FIFO *cache;
};
