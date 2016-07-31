#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <vector>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericDatabaseObject.h"
#include "GenericObjectManager.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class Player : public GenericDatabaseObject<RankingDb>
  {
    friend class PlayerMngr;
    friend class SqliteOverlay::GenericObjectManager<RankingDb>;

  public:
    string getName__FirstNameFirst() const;
    string getName__LastNameFirst() const;
    string getLastName() const;
    string getFirstName() const;

  private:
    Player(RankingDb* db, int rowId);
    Player(RankingDb* db, TabRow row);
  };

  typedef unique_ptr<Player> upPlayer;
  typedef vector<Player> PlayerList;
}

#endif  /* PLAYER_H */
