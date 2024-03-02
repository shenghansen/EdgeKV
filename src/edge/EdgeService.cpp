#include "edge/EdgeService.h"

EdgeService::EdgeService() {
  this->client = new KVClient();
  this->db = new Level_db("tmp/client");
}

EdgeService::~EdgeService() {
  delete this->client;
  delete this->db;
}
