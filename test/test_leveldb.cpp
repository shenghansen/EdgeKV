#include "utill/leveldb.h"
#include <cassert>
#include <iostream>

void test_leveldb() {
  leveldb::DB *db;
  leveldb::Options options;
  options.create_if_missing = true;
  // options.error_if_exists = true;
  leveldb::Status status = leveldb::DB::Open(options, "/tmp/server", &db);
  if (!status.ok())
    std::cout << "open failed" << status.ToString() << std::endl;

  std::string key = "key";
  std::string value = "value";
  status = db->Put(leveldb::WriteOptions(), key, value);
  db->Put(leveldb::WriteOptions(), "a", value);
  db->Put(leveldb::WriteOptions(), "b", value);
  db->Put(leveldb::WriteOptions(), "c", value);
  db->Put(leveldb::WriteOptions(), "d", value);
  db->Put(leveldb::WriteOptions(), "e", value);
  db->Put(leveldb::WriteOptions(), "f", value);
  db->Put(leveldb::WriteOptions(), "d", value);
  db->Put(leveldb::WriteOptions(), "aaa", value);
  assert(status.ok());
  std::cout << "put ok" << std::endl;

  status = db->Get(leveldb::ReadOptions(), key, &value);
  assert(status.ok());
  assert(value == "value");

  // 创建迭代器
  leveldb::Iterator *it = db->NewIterator(leveldb::ReadOptions());
  it->Seek("a");
  int n = 4;
  int count = 0;
  while (it->Valid() && count < n) {
    std::string currentKey = it->key().ToString();
    std::string currentValue = it->value().ToString();

    // 跳过当前键
    if (currentKey == key) {
      it->Next();
      continue;
    }

    // 处理相邻键值对
    std::cout << "Key: " << currentKey << ", Value: " << currentValue
              << std::endl;

    it->Next();
    count++;
  }

  // 检查迭代器是否出错
  if (!it->status().ok()) {
    // 处理迭代器错误
  }

  status = db->Delete(leveldb::WriteOptions(), key);
  assert(status.ok());
  delete it;

  delete db;
}

int main(int argc, char const *argv[]) {
  // Level_db db("/tmp/server");
  // db.put("a", "b");
  // std::string value;
  // db.get("a", value);
  // std::cout << value << std::endl;
  test_leveldb();
  return 0;
}
