#ifndef MATCHMNGR_H
#define MATCHMNGR_H

#include <memory>
#include <vector>

#include <boost/date_time/gregorian/gregorian.hpp>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericObjectManager.h"
#include "Player.h"
#include "Match.h"
#include "Score.h"
#include "RankingDataDefs.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class RankingSystem;

  class MatchMngr : public GenericObjectManager<RankingDb>
  {
  public:
    MatchMngr(RankingDb* _db, RankingSystem* _rs);

    // create
    upMatch stageNewMatch_Singles(const Player& player1, const Player& player2, MatchScore& score, const boost::gregorian::date& date, ERR* err=nullptr) const;
    ERR confirmMatch(const Match& ma) const;

    // getters
    upMatch getMatchById(int id) const;
    upMatch getLatestMatchForPlayer(const Player& p, const RANKING_CLASS& rankClass, bool confirmedMatchesOnly=true) const;


    // comparison functions for std::sort
    //static std::function<bool (Player&, Player&)> getPlayerSortFunction_byLastName();

  private:
    RankingSystem* rs;

  };
}

#endif  /* MATCHMNGR_H */
