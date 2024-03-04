#include "edge/EdgeService.h"

EdgeService::EdgeService() : cache(1ULL << 10) {}

EdgeService::~EdgeService() {
  // delete this->client;
  // delete this->db;
}

void EdgeService::run() {}
