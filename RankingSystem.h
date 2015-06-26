#ifndef RANKINGSYS_H
#define RANKINGSYS_H

#include <memory>
#include <string>

#include "RankingDb.h"
#include "RankingErrCodes.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class RankingSystem
  {
  public:
    static unique_ptr<RankingSystem> get(const string& fname, ERR* err);
    static unique_ptr<RankingSystem> createEmpty(const string& fname, ERR* err);
    static bool isValidFilename(const string& fname);

  protected:
    upRankingDb db;
    RankingSystem(upRankingDb _db);
    static unique_ptr<RankingSystem> doInit(const string& fname, bool doCreateNew, ERR* err);
  };
}

#endif  /* RANKINGSYS_H */
