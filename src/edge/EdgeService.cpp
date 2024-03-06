#include "edge/EdgeService.h"
// // EdgeService
// EdgeService::EdgeService(const std::string &server,
//                          const std::string &load_balancer,
//                          const std::string &protocol,
//                          const std::string &connection_type, int timeout_ms,
//                          int max_retry, unsigned long cache_size) {
//   cache = new S3FIFO(server, load_balancer, protocol, connection_type,
//                      timeout_ms, max_retry << 10);
//   updateClientService = new UpdateClientServiceImpl(cache);
//   httpService = new HttpServiceImpl(cache);
//   // if (this->server.AddService(updateClientService,
//   //                             brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
//   //   LOG(ERROR) << "Fail to add listen_datacenter_service";
//   // }

//   if (this->server.AddService(httpService, brpc::SERVER_DOESNT_OWN_SERVICE)
//   !=
//       0) {
//     LOG(ERROR) << "Fail to add httpservice";
//   }
// }

// EdgeService::~EdgeService() {
//   delete updateClientService;
//   delete httpService;
//   delete cache;
// }

// void EdgeService::run() {
//   butil::EndPoint point;
//   std::string FLAGS_listen_addr = "";
//   if (!FLAGS_listen_addr.empty()) {
//     if (butil::str2endpoint(FLAGS_listen_addr.c_str(), &point) < 0) {
//       LOG(ERROR) << "Invalid listen address:" << FLAGS_listen_addr;
//     }
//   } else {
//     point = butil::EndPoint(butil::IP_ANY, 8000);
//   }
//   brpc::ServerOptions options;
//   options.idle_timeout_sec = -1;
//   if (server.Start(point, &options) != 0) {
//     LOG(ERROR) << "Fail to start KVServer";
//   }
//   server.RunUntilAskedToQuit();
//   // std::thread edgeKV(
//   //   [&]() { server.RunUntilAskedToQuit(); });
// }

// UpdateClientServiceImpl
UpdateClientServiceImpl::UpdateClientServiceImpl(S3FIFO *cache) {
  this->cache = cache;
}

void UpdateClientServiceImpl::Update(google::protobuf::RpcController *cntl_base,
                                     const UpdateClientRequest *request,
                                     UpdateClientResponse *response,
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
  cache->updateCache(request->exist(), request->key(), request->value());
}

// HttpServiceImpl
HttpServiceImpl::HttpServiceImpl(S3FIFO *cache) { this->cache = cache; }

void HttpServiceImpl::Get(google::protobuf::RpcController *cntl_base,
                          const HttpRequest *, HttpResponse *,
                          google::protobuf::Closure *done) {
  {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller *>(cntl_base);
    const std::string *_key = cntl->http_request().uri().GetQuery("key");
    if (_key == nullptr) {
      INFO("http get receive empty key");
      return;
    }
    std::string key = *_key;
    std::string value;
    cache->get(key, value);
    INFO("http get {}", key);

    // body是纯文本
    cntl->http_response().set_content_type("text/plain");

    // 把请求的query-string和body打印结果作为回复内容。

    // INFO("query:");
    // for (brpc::URI::QueryIterator it =
    // cntl->http_request().uri().QueryBegin();
    //      it != cntl->http_request().uri().QueryEnd(); ++it) {
    //   INFO("{}={}", it->first, it->second);
    // }
    // INFO("body:{}", cntl->request_attachment());
    // INFO("query:");
    // INFO("\nbody: {}", cntl->request_attachment());
    butil::IOBufBuilder os;
    os << "key:" << key.c_str() << "value" << value.c_str();
    os.move_to(cntl->response_attachment());
  }
}

void HttpServiceImpl::Put(google::protobuf::RpcController *cntl_base,
                          const HttpRequest *, HttpResponse *,
                          google::protobuf::Closure *done) {
  {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller *>(cntl_base);

    const std::string *_key = cntl->http_request().uri().GetQuery("key");
    const std::string *_value = cntl->http_request().uri().GetQuery("value");
    if (_key == nullptr || _value == nullptr) {
      INFO("http get receive empty key or value");
      return;
    }
    std::string key = *_key;
    std::string value = *_value;
    cache->get(key, value);
    INFO("http put {}:{}", key, value);

    // body是纯文本
    cntl->http_response().set_content_type("text/plain");

    butil::IOBufBuilder os;
    os << "key:" << key.c_str() << "value" << value.c_str();
    os.move_to(cntl->response_attachment());
  }
}

void HttpServiceImpl::Del(google::protobuf::RpcController *cntl_base,
                          const HttpRequest *, HttpResponse *,
                          google::protobuf::Closure *done) {
  {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller *>(cntl_base);

    const std::string *_key = cntl->http_request().uri().GetQuery("key");
    if (_key == nullptr) {
      INFO("http del receive empty key");
      return;
    }
    std::string key = *_key;
    bool status = cache->del(key);
    INFO("http del {}", key);

    cntl->http_response().set_content_type("text/plain");

    butil::IOBufBuilder os;
    os << "status:" << status;
    os.move_to(cntl->response_attachment());
  }
}
