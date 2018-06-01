
#include <framework/database/database.hpp>

#include <framework/sqlite3.h>

using namespace db;

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

database::database() {
  int rc = sqlite3_open("test.sqlite3", &db);
  if (rc != SQLITE_OK ){
    sqlite3_close(db);
    //return(1);
  }
}

database::~database() {
  sqlite3_close(db);
}

int 
database::exec(const std::string& cmd) {
  char *zErrMsg = 0;
  int ret = sqlite3_exec(db, cmd.c_str(), callback, 0, &zErrMsg);
  if (ret) {
  	error = zErrMsg;
  }
  return ret;
}