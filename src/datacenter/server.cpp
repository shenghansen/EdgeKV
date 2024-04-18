#include "datacenter/server.h"

KVServiceImpl::KVServiceImpl() {
  db = new Level_db("/tmp/server");
  this->options.protocol = "baidu_std";
  this->options.connection_type = "";
  this->options.timeout_ms = 100000;
  this->options.max_retry = 3;
}

KVServiceImpl::~KVServiceImpl() {
  delete db;
  delete stub;
}

void KVServiceImpl::Get(google::protobuf::RpcController *cntl_base,
                        const GetRequest *request, GetResponse *response,
                        google::protobuf::Closure *done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller *cntl = static_cast<brpc::Controller *>(cntl_base);

  // optional callback
  cntl->set_after_rpc_resp_fn(
      std::bind(&KVServiceImpl::CallAfterRpc, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  LOG(INFO) << "Received request[log_id=" << cntl->log_id() << "] from "
            << cntl->remote_side() << " to " << cntl->local_side() << ": "
            << request->key() << " (attached=" << cntl->request_attachment()
            << ")";

  std::string value;
  if (db->get(request->key(), value)) {
    response->set_value(value);
    response->set_exist(true);
  } else {
    response->set_exist(false);
    response->set_value("");
  }
  response->set_status(true);
}

void KVServiceImpl::GetRange(google::protobuf::RpcController *cntl_base,
                             const GetRequest *request,
                             GetRangeResponse *response,
                             google::protobuf::Closure *done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller *cntl = static_cast<brpc::Controller *>(cntl_base);

  // optional callback
  cntl->set_after_rpc_resp_fn(
      std::bind(&KVServiceImpl::CallAfterRpc, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  LOG(INFO) << "Received request[log_id=" << cntl->log_id() << "] from "
            << cntl->remote_side() << " to " << cntl->local_side() << ": "
            << request->key() << " (attached=" << cntl->request_attachment()
            << ")";
  std::string value;
  size_t num;
  std::string *key_list = new std::string[PREFETCH_RANGE];
  std::string *value_list = new std::string[PREFETCH_RANGE];
  if (!db->get(request->key(), value)) {
    response->set_exist(false);
    response->set_status(true);
    response->set_num(1);
    return;
  }
  response->set_exist(true);
  response->set_status(
      db->get_range(num, request->key(), key_list, value_list));
  response->set_num(num);
  for (size_t i = 0; i < num; i++) {
    response->add_key(key_list[i]);
    response->add_value(value_list[i]);
  }
}

void KVServiceImpl::Put(google::protobuf::RpcController *cntl_base,
                        const PutRequest *request, PutResponse *response,
                        google::protobuf::Closure *done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller *cntl = static_cast<brpc::Controller *>(cntl_base);

  // optional callback
  cntl->set_after_rpc_resp_fn(
      std::bind(&KVServiceImpl::CallAfterRpc, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  LOG(INFO) << "Received request[log_id=" << cntl->log_id() << "] from "
            << cntl->remote_side() << " to " << cntl->local_side() << ": "
            << "Put:" << request->key() << ", " << request->value()
            << " (attached=" << cntl->request_attachment() << ")";

  std::string value;
  response->set_status(db->put(request->key(), request->value()));
}

void KVServiceImpl::Del(google::protobuf::RpcController *cntl_base,
                        const DelRequest *request, DelResponse *response,
                        google::protobuf::Closure *done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller *cntl = static_cast<brpc::Controller *>(cntl_base);

  // optional callback
  cntl->set_after_rpc_resp_fn(
      std::bind(&KVServiceImpl::CallAfterRpc, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  LOG(INFO) << "Received request[log_id=" << cntl->log_id() << "] from "
            << cntl->remote_side() << " to " << cntl->local_side() << ": "
            << request->key() << " (attached=" << cntl->request_attachment()
            << ")";

  if (db->del(request->key())) {
    response->set_status(true);
  } else
    response->set_status(false);
}

bool KVServiceImpl::UpdateClient(std::string &client_address, bool exist,
                                 std::string &key, std::string &value) {
  UpdateClientRequest request;
  UpdateClientResponse response;
  brpc::Controller cntl;
  request.set_exist(exist);
  request.set_key(key);
  request.set_value(value);

  brpc::Channel channel;
  UpdateClientService_Stub *stub;
  if (channel.Init("0.0.0.0:18808", "", &options) != 0) {
    LOG(ERROR) << "Fail to initialize channel";
    return false;
  }
  UpdateClientService_Stub stup(&channel);

  stub->Update(&cntl, &request, &response, NULL);
  if (!cntl.Failed()) {
    LOG(INFO) << "Received response from " << cntl.remote_side() << " to "
              << cntl.local_side() << ": " << response.status()
              << " (attached=" << cntl.response_attachment() << ")"
              << " latency=" << cntl.latency_us() << "us";
    if (response.status()) {
      return response.status();
    }
  }
  WARN("Update client Failed ErrorText:{}", cntl.ErrorText());
  return false;
}
