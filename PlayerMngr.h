#ifndef PLAYERMNGR_H
#define PLAYERMNGR_H

#include <memory>
#include <vector>

#include <boost/date_time/gregorian/gregorian.hpp>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericObjectManager.h"
#include "Player.h"
#include "ValidityPeriod.h"

using namespace SqliteOverlay;
using namespace boost::gregorian;

namespace RankingApp {

  class RankingClass;

  class PlayerMngr : public GenericObjectManager<RankingDb>
  {
  public:
    PlayerMngr(RankingDb* _db);

    // create
    upPlayer createNewPlayer(const string& firstName, const string& lastName, ERR* err=nullptr) const;

    // getters
    upPlayer getPlayerByName(const string& firstName, const string& lastName) const;
    upPlayer getPlayerById(int id) const;
    vector<ValidityPeriod> getValidityPeriodsForPlayer_Global(const Player& p) const;
    unique_ptr<date> getEarliestActivationDateForPlayer_Global(const Player& p) const;
    unique_ptr<date> getLatestDeactivationDateForPlayer_Global(const Player& p) const;
    PlayerList getAllPlayers() const;
    PlayerList getActivePlayersOnGivenDate(const RankingClass& rankClass, const date& d) const;


    // modify player validity / activity
    ERR enablePlayer(const Player& p, const RankingClass& rankClass, const date& startDate, bool skipInitialScore=false) const;
    ERR disablePlayer(const Player& p, const RankingClass& rankClass, const date& endDate) const;
    bool isPlayerEnabledOnSpecificDate(const Player& p, const RankingClass& rankClass, const date& d) const;

    // comparison functions for std::sort
    static std::function<bool (Player&, Player&)> getPlayerSortFunction_byLastName();

  private:
    DbTab* validityTab;

  };
}

#endif  /* PLAYERMNGR_H */
