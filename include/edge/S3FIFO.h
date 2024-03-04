#include "client.h"
#include "utill/utill.h"
#include <cstdint>
#include <queue>
#include <sys/param.h>
#include <unordered_map>
using std::string;

struct cache_item {
  string value;
  uint8_t freq = 0;
  bool is_exists = false; // Does the data exist or not
  cache_item(string v, bool i) : value(v), is_exists(i) {}
};

struct ghost_item {
  size_t timestamp;
  ghost_item(size_t t) : timestamp(t) {}
};

class S3FIFO {
public:
  using Queue = std::queue<string>;
  using Cache = std::unordered_map<string, cache_item *>;
  using Ghost = std::unordered_map<string, ghost_item>;
  S3FIFO(unsigned long cache_size);
  ~S3FIFO();
  bool get(string &key, string &value);
  bool put(string &key, string &value);
  bool del(string &key);
  void insert(string &key, cache_item *c_i);
  void evictM();
  void evictS();

private:
  // rpc
  KVClient *client;
  // cache
  size_t cache_size;
  size_t s_cache_size;
  size_t m_cache_size;
  Cache cache;
  Queue S;
  Queue M;
  Ghost G;
  // ghost
  size_t ghost_timestamp = 0;
  // for test
  size_t miss = 0;
  size_t hit = 0;
};
