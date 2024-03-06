#include "edge/client.h"
#include "utill/KVService.pb.h"

KVClient::KVClient(const std::string &server, const std::string &load_balancer,
                   const std::string &protocol,
                   const std::string &connection_type, int timeout_ms,
                   int max_retry) {
  this->options.protocol = protocol;
  this->options.connection_type = connection_type;
  this->options.timeout_ms = timeout_ms;
  this->options.max_retry = max_retry;
  if (channel.Init(server.c_str(), load_balancer.c_str(), &options) != 0) {
    LOG(ERROR) << "Fail to initialize channel";
    return;
  }
  this->stub = new KVService_Stub(&channel);
}

KVClient::~KVClient() { delete stub; }

bool KVClient::put(const std::string &key, const std::string &value) {
  PutRequest request;
  PutResponse response;
  brpc::Controller cntl;
  request.set_key(key);
  request.set_value(value);

  this->stub->Put(&cntl, &request, &response, NULL);
  if (!cntl.Failed()) {
    LOG(INFO) << "Received response from " << cntl.remote_side() << " to "
              << cntl.local_side() << ": " << response.status()
              << " (attached=" << cntl.response_attachment() << ")"
              << " latency=" << cntl.latency_us() << "us";
    if (response.status()) {
      return true;
    }
  }
  WARN("PUT Failed ErrorText:{}", cntl.ErrorText());
  return false;
}

bool KVClient::get(bool &exist, const std::string &key, std::string &value) {
  GetRequest request;
  GetResponse response;
  brpc::Controller cntl;
  request.set_key(key);

  this->stub->Get(&cntl, &request, &response, NULL);
  if (!cntl.Failed()) {
    LOG(INFO) << "Received response from " << cntl.remote_side() << " to "
              << cntl.local_side() << ": " << response.status()
              << " (attached=" << cntl.response_attachment() << ")"
              << " latency=" << cntl.latency_us() << "us";
    if (response.status()) {
      exist = response.exist();
      value = response.value();
      return true;
    }
  }
  WARN("GET Failed ErrorText:{}", cntl.ErrorText());
  return false;
}

bool KVClient::getRange(bool &exist, int &num, const std::string &key,
                        std::string *key_list, std::string *value_list) {
  GetRequest request;
  GetRangeResponse response;
  brpc::Controller cntl;
  request.set_key(key);

  this->stub->GetRange(&cntl, &request, &response, NULL);
  if (!cntl.Failed()) {
    LOG(INFO) << "Received response from " << cntl.remote_side() << " to "
              << cntl.local_side() << ": " << response.status()
              << " (attached=" << cntl.response_attachment() << ")"
              << " latency=" << cntl.latency_us() << "us";
    if (response.status()) {
      exist = response.exist();
      if (exist) {
        num = response.num();
        for (size_t i = 0; i < num; i++) {
          key_list[i] = response.key(i);
          value_list[i] = response.value(i);
        }
      }
      return true;
    }
  }
  WARN("GET Failed ErrorText:{}", cntl.ErrorText());
  return false;
}
bool KVClient::del(const std::string &key) {
  DelRequest request;
  DelResponse response;
  brpc::Controller cntl;
  request.set_key(key);

  this->stub->Del(&cntl, &request, &response, NULL);
  if (!cntl.Failed()) {
    LOG(INFO) << "Received response from " << cntl.remote_side() << " to "
              << cntl.local_side() << ": " << response.status()
              << " (attached=" << cntl.response_attachment() << ")"
              << " latency=" << cntl.latency_us() << "us";
    if (response.status()) {
      return true;
    }
  }
  WARN("GET Failed ErrorText:{}", cntl.ErrorText());
  return false;
}
