#include "S3FIFO.h"
#include "utill/leveldb.h"
#include <unordered_set>
class EdgeService {
public:
  EdgeService();
  ~EdgeService();
  void run();

private:
  S3FIFO cache;

  // unused
  // KVClient *client;
  // Level_db *db;
  // std::unordered_set<std::string> uset;
};