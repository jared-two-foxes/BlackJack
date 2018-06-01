#ifndef FRAMEWORK_DATABASE_HPP__
#define FRAMEWORK_DATABASE_HPP__

#include <string>

// forward declaration
typedef struct sqlite3 sqlite3;

namespace db {

struct database {
  sqlite3* db;
  std::string error;
  
  database();
  ~database();

  int exec(const std::string& cmd);
};	

}

#endif // FRAMEWORK_DATABASE_HPP__