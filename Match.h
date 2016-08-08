#ifndef MATCH_H
#define MATCH_H

#include <memory>
#include <string>
#include <vector>

#include "RankingDb.h"
#include "RankingDataDefs.h"
#include "RankingErrCodes.h"
#include "GenericDatabaseObject.h"
#include "GenericObjectManager.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class Match : public GenericDatabaseObject<RankingDb>
  {
    friend class MatchMngr;
    friend class SqliteOverlay::GenericObjectManager<RankingDb>;

  public:
    MatchState getState() const;
    bool isDoubles() const;
    greg::date getMatchTime() const;

  private:
    Match(RankingDb* db, int rowId);
    Match(RankingDb* db, TabRow row);
  };

  typedef unique_ptr<Match> upMatch;
  typedef vector<Match> MatchList;
}

#endif  /* MATCH_H */
