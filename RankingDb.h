#ifndef RANKINGDB_H
#define RANKINGDB_H

#include "SqliteDatabase.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class RankingDb : public SqliteDatabase
  {
  public:
    static constexpr int MAX_NAME_LEN = 40;

    virtual void populateTables();
    virtual void populateViews();

    RankingDb() : SqliteDatabase() {}
    RankingDb(string sqliteFilename, bool createNew) : SqliteDatabase(sqliteFilename, createNew) {}

  };
}

#endif  /* RANKINGDB_H */
