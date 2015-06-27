#ifndef PLAYERMNGR_H
#define PLAYERMNGR_H

#include <memory>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericObjectManager.h"
#include "Player.h"

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

  private:
    RankingDb* db;
    DbTab* playerTab;

  };
}

#endif  /* PLAYERMNGR_H */
