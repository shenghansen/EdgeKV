#include "client.h"
#include "utill/leveldb.h"
class EdgeService {
public:
  EdgeService();
  ~EdgeService();

  // private:
  KVClient *client;
  Level_db *db;
};