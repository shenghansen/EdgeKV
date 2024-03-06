#include "edge/S3FIFO.h"

S3FIFO::S3FIFO(const std::string &server, const std::string &load_balancer,
               const std::string &protocol, const std::string &connection_type,
               int timeout_ms, int max_retry, unsigned long cache_size) {
  this->cache_size = cache_size;
  s_cache_size = cache_size / 10;
  m_cache_size = cache_size - s_cache_size;
  INFO("created S3FIFO cache, cache_size:{}, s_cache_size:{}, m_cache_size:{}",
       cache_size, s_cache_size, m_cache_size);

  this->client = new KVClient(server, load_balancer, protocol, connection_type,
                              timeout_ms, max_retry);
}
S3FIFO::~S3FIFO() { delete client; }

bool S3FIFO::get(string &key, string &value) {
  auto it = cache.find(key);
  if (it != cache.end()) {
    // cache hit
    hit++;
    it->second->freq = MIN(it->second->freq + 1, 3);
    if (it->second->is_exists)
      value = it->second->value;
    else {
      value = "";
      return false;
    }
    INFO("get hit cache ");
    return true;
  } else {
    // cache miss
    miss++;
    // request from datacenter
    cache_item *c_i;
    bool flag = client->get(key, value);
    if (flag)
      // datacenter has KV
      c_i = new cache_item(value, true);
    else
      // datacenter don't have KV
      c_i = new cache_item("", false);
    // add data to cache
    insert(key, c_i);
    INFO("get miss cache,request from datacenter");
    return flag;
  }
  return false;
}

bool S3FIFO::put(string &key, string &value) {
  auto it = cache.find(key);
  if (it != cache.end()) {
    // cache hit,KV already exists
    hit++;
    if (!it->second->is_exists) {
      // if KV don't exist
      bool flag = client->put(key, value);
      it->second->freq = MIN(it->second->freq + 1, 3);
      it->second->is_exists = flag;
      it->second->value = value;
      INFO("put hit cache,KV don't exists");
    }
    INFO("put hit cache,KV already exists");
    return false;
  } else {
    // cache miss
    miss++;
    bool flag = client->put(key, value);
    cache_item *c_i;
    if (flag)
      // put KV success
      c_i = new cache_item(value, true);
    else
      // put KV failed
      c_i = new cache_item("", false);
    // add data to cache
    insert(key, c_i);
    INFO("put miss cache,put to datacenter");
    return flag;
  }
  return false;
}

bool S3FIFO::del(string &key) {
  // delete KV through rpc
  bool flag = client->del(key);
  auto it = cache.find(key);
  if (it != cache.end()) {
    // cache hit
    hit++;
    it->second->freq = MIN(it->second->freq + 1, 3);
    if (flag)
      // delete sucess,add negative search to local cache
      it->second->is_exists = false;
    // delete failed,return false
    INFO("del hit cache");
  } else {
    // cache miss
    miss++;
    if (flag) {
      // delete sucess,add negative search to local cache
      cache_item *c_i = new cache_item("", false);
      // add data to cache
      insert(key, c_i);
    }
    // delete failed,return false
    INFO("del miss cache");
  }
  return flag;
}

void S3FIFO::insert(string &key, cache_item *c_i) {
  // if cache is full
  while (S.size() + M.size() >= cache_size) {
    if (M.size() > m_cache_size)
      evictM();
    else
      evictS();
  }
  auto it = G.find(key);
  if (it != G.end())
    // in Ghost queue
    M.push(key);
  else
    // not in Ghost queue
    S.push(key);
  // add data to cache
  cache.insert({key, c_i});
}

void S3FIFO::evictM() {
  while (!M.empty()) {
    auto key = M.front();
    auto it = cache.find(key);
    M.pop();
    if (it->second->freq > 0) {
      // if freq>0,reinsert to M
      it->second->freq -= 1;
      M.push(key);
    } else {
      // delete from cache add to Ghost
      delete it->second;
      cache.erase(key);
      // G is full
      // TODO
      if (G.size() >= m_cache_size) {
        for (auto it : G) {
          if (it.second.timestamp <= ghost_timestamp - m_cache_size) {
            G.erase(it.first);
          }
        }
      }
      G.insert({key, ghost_item(ghost_timestamp++)});
      return;
    }
  }
}

void S3FIFO::evictS() {
  while (!S.empty()) {
    auto key = S.front();
    auto it = cache.find(key);
    if (it->second->freq > 0) {
      // if freq>0,insert to M
      it->second->freq = 0;
      M.push(key);
    } else {
      // delete from cache add to Ghost
      delete it->second;
      cache.erase(key);
      // G is full
      // TODO
      if (G.size() >= m_cache_size) {
        for (auto it : G) {
          if (it.second.timestamp <= ghost_timestamp - m_cache_size) {
            G.erase(it.first);
          }
        }
      }
      G.insert({key, ghost_item(ghost_timestamp++)});
      return;
    }
  }
}

void S3FIFO::updateCache(bool exist, const string &key, const string &value) {
  auto it = cache.find(key);
  if (it != cache.end()) {
    it->second->is_exists = exist;
    it->second->value = value;
  }
}
