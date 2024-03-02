#include "utill/leveldb.h"
#include <filesystem>

namespace fs = std::filesystem;

bool directoryExists(const std::string &path) {
  return fs::exists(path) && fs::is_directory(path);
}

Level_db::Level_db(const char *path) {
  options.create_if_missing = true;
  // options.error_if_exists = true;
  leveldb::Status status = leveldb::DB::Open(options, path, &db);
  if (!status.ok()) {
    WARN("open leveldb failed");
    WARN(status.ToString());
  } else
    INFO("open leveldb successed");
}

Level_db::~Level_db() {
  delete db;
  INFO("close db");
}

bool Level_db::put(const std::string &key, const std::string &value) {
  leveldb::Status status = db->Put(leveldb::WriteOptions(), key, value);
  if (!status.ok()) {
    WARN("put leveldb failed");
    WARN(status.ToString());
  } else
    INFO("put {},{}", key, value);
  return status.ok();
}

bool Level_db::get(const std::string &key, std::string &value) {
  leveldb::Status status = db->Get(leveldb::ReadOptions(), key, &value);
  if (!status.ok()) {
    WARN("get failed");
    WARN(status.ToString());
  } else
    INFO("get {},{}", key, value);
  return status.ok();
}

bool Level_db::del(const std::string &key) {
  leveldb::Status status = db->Delete(leveldb::WriteOptions(), key);
  if (!status.ok()) {
    WARN("del failed");
    WARN(status.ToString());
  } else
    INFO("del {}", key);
  return status.ok();
}
