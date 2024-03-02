#include "utill/leveldb.h"
#include <cassert>
#include <iostream>

void test_leveldb() {
  leveldb::DB *db;
  leveldb::Options options;
  options.create_if_missing = true;
  options.error_if_exists = true;
  leveldb::Status status = leveldb::DB::Open(options, "/tmp/server", &db);
  if (!status.ok())
    std::cout << status.ToString() << std::endl;

  std::string key = "key";
  std::string value = "value";
  status = db->Put(leveldb::WriteOptions(), key, value);
  assert(status.ok());

  status = db->Get(leveldb::ReadOptions(), key, &value);
  assert(status.ok());
  assert(value == "value");

  status = db->Delete(leveldb::WriteOptions(), key);
  assert(status.ok());

  delete db;
}

int main(int argc, char const *argv[]) {
  Level_db db("/tmp/server");
  db.put("a", "b");
  std::string value;
  db.get("a", value);
  std::cout << value << std::endl;

  return 0;
}
