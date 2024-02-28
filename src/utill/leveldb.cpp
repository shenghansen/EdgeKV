#include "utill/leveldb.h"

Level_db::Level_db(const char *path) {
  options.create_if_missing = true;
  options.error_if_exists = true;
  leveldb::Status status = leveldb::DB::Open(options, path, &db);
  if (!status.ok()) {
    ERROR("open leveldb failed");
    ERROR(status.ToString());
  } else
    LOG("open leveldb successed");
}

Level_db::~Level_db() {
  delete db;
  LOG("close db");
}

bool Level_db::put(const char *key, const char *value) {
  leveldb::Status status = db->Put(leveldb::WriteOptions(), key, value);
  if (!status.ok()) {
    ERROR("put leveldb failed");
    ERROR(status.ToString());
  } else
    LOG("put {},{}", key, value);
  return status.ok();
}

bool Level_db::get(const char *key, std::string &value) {
  leveldb::Status status = db->Get(leveldb::ReadOptions(), key, &value);
  if (!status.ok()) {
    ERROR("get leveldb failed");
    ERROR(status.ToString());
  } else
    LOG("get {},{}", key, value);
  return status.ok();
}

bool Level_db::del(const char *key) {
  leveldb::Status status = db->Delete(leveldb::WriteOptions(), key);
  if (!status.ok()) {
    ERROR("del leveldb failed");
    ERROR(status.ToString());
  } else
    LOG("del {}", key);
  return status.ok();
}
