#ifndef MATCHMNGR_H
#define MATCHMNGR_H

#include <memory>
#include <vector>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericObjectManager.h"
#include "Player.h"
#include "Match.h"
#include "Score.h"
#include "RankingDataDefs.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class MatchMngr : public GenericObjectManager
  {
  public:
    MatchMngr(RankingDb* _db);

    // create
    upMatch stageNewMatch_Singles(const Player& player1, const Player& player2, MatchScore& score, const LocalTimestamp& timestamp, ERR* err=nullptr) const;

    // getters
    upMatch getMatchById(int id) const;
    upMatch getLatestMatchForPlayer(const Player& p, const RANKING_CLASS& rankClass) const;


    // comparison functions for std::sort
    //static std::function<bool (Player&, Player&)> getPlayerSortFunction_byLastName();

  private:
    RankingDb* db;
    DbTab* matchTab;

  };
}

#endif  /* MATCHMNGR_H */
