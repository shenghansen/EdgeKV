#include "edge/EdgeService.h"

EdgeService::EdgeService(const std::string &server,
                         const std::string &load_balancer,
                         const std::string &protocol,
                         const std::string &connection_type, int timeout_ms,
                         int max_retry, unsigned long cache_size)
    : cache(server, load_balancer, protocol, connection_type, timeout_ms,
            max_retry << 10),
      listen_datacenter_server() {
  updateClientService = new UpdateClientServiceImpl();
  if (this->listen_datacenter_server.AddService(
          updateClientService, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
    LOG(ERROR) << "Fail to add service";
  }
}

EdgeService::~EdgeService() {
  // delete this->client;
  // delete this->db;
  delete updateClientService;
}

void EdgeService::run() {
  std::thread listen_data_center(
      [&]() { listen_datacenter_server.RunUntilAskedToQuit(); });
}

void UpdateClientServiceImpl::Update(google::protobuf::RpcController *cntl_base,
                                     const GetRequest *request,
                                     GetResponse *response,
                                     google::protobuf::Closure *done) {
  brpc::ClosureGuard done_guard(done);
  brpc::Controller *cntl = static_cast<brpc::Controller *>(cntl_base);

  // optional callback
  cntl->set_after_rpc_resp_fn(
      std::bind(&UpdateClientServiceImpl::CallAfterRpc, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  LOG(INFO) << "Received request[log_id=" << cntl->log_id() << "] from "
            << cntl->remote_side() << " to " << cntl->local_side() << ": "
            << request->key() << " (attached=" << cntl->request_attachment()
            << ")";

  response->set_status(true);
}
