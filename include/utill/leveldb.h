#include "utill.h"
#include <cassert>
#include <leveldb/db.h>
#include <string>

class Level_db {
private:
  leveldb::DB *db;
  leveldb::Options options;

public:
  Level_db(const char *path);
  ~Level_db();
  bool put(const std::string &key, const std::string &value);
  bool get(const std::string &key, std::string &value);
  bool del(const std::string &key);
};
