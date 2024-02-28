#include "utill.h"
#include <cassert>
#include <leveldb/db.h>

class Level_db {
private:
  leveldb::DB *db;
  leveldb::Options options;

public:
  Level_db(const char *path);
  ~Level_db();
  bool put(const char *key, const char *value);
  bool get(const char *key, std::string &value);
  bool del(const char *key);
};
