#ifndef PLAYERMNGR_H
#define PLAYERMNGR_H

#include <memory>
#include <vector>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericObjectManager.h"
#include "Player.h"
#include "ValidityPeriod.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class PlayerMngr : public GenericObjectManager
  {
  public:
    PlayerMngr(RankingDb* _db);

    // create
    upPlayer createNewPlayer(const string& firstName, const string& lastName, ERR* err=nullptr) const;

    // getters
    upPlayer getPlayerByName(const string& firstName, const string& lastName) const;
    upPlayer getPlayerById(int id) const;
    vector<ValidityPeriod> getValidityPeriodsForPlayer(const Player& p) const;
    upLocalTimestamp getEarliestActivationDateForPlayer(const Player& p) const;
    upLocalTimestamp getLatestDeactivationDateForPlayer(const Player& p) const;
    PlayerList getAllPlayers() const;


    // modify player validity / activity
    ERR enablePlayer(const Player& p, int startYear, int startMonth, int startDay) const;
    ERR disablePlayer(const Player& p, int endYear, int endMonth, int endDay) const;
    bool isPlayerEnabledOnSpecificDate(const Player& p, int year, int month, int day) const;
    bool isPlayerEnabledOnSpecificDate(const Player& p, const string& isoDate) const;

    // comparison functions for std::sort
    static std::function<bool (Player&, Player&)> getPlayerSortFunction_byLastName();

  private:
    RankingDb* db;
    DbTab* playerTab;
    DbTab* validityTab;

  };
}

#endif  /* PLAYERMNGR_H */
