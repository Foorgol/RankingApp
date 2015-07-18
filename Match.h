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

  class Match : public GenericDatabaseObject
  {
    friend class MatchMngr;
    friend class SqliteOverlay::GenericObjectManager;

  public:
    MATCH_STATE getState() const;
    static int MatchStateToInt(MATCH_STATE state);
    static MATCH_STATE intToMatchState(int state);
    bool isDoubles() const;
    LocalTimestamp getMatchTime() const;

  private:
    Match(SqliteDatabase* db, int rowId);
    Match(SqliteDatabase* db, TabRow row);
  };

  typedef unique_ptr<Match> upMatch;
  typedef vector<Match> MatchList;
}

#endif  /* MATCH_H */
