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
    MATCH_STATE getState() const;
    static int MatchStateToInt(MATCH_STATE state);
    static MATCH_STATE intToMatchState(int state);
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
