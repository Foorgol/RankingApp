#ifndef RANKINGSYS_H
#define RANKINGSYS_H

#include <memory>
#include <string>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "PlayerMngr.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class RankingSystem
  {
  public:
    static unique_ptr<RankingSystem> get(const string& fname, ERR* err);
    static unique_ptr<RankingSystem> createEmpty(const string& fname, ERR* err);
    static bool isValidFilename(const string& fname);

    PlayerMngr getPlayerMngr() const;

  protected:
    upRankingDb db;
    RankingSystem(upRankingDb _db);
    static unique_ptr<RankingSystem> doInit(const string& fname, bool doCreateNew, ERR* err);
  };

  typedef unique_ptr<RankingSystem> upRankingSystem;
}

#endif  /* RANKINGSYS_H */
