#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericDatabaseObject.h"
#include "GenericObjectManager.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class Player : public GenericDatabaseObject
  {
    friend class PlayerMngr;
    friend class SqliteOverlay::GenericObjectManager;

  public:

  private:
    Player(SqliteDatabase* db, int rowId);
    Player(SqliteDatabase* db, TabRow row);
  };

  typedef unique_ptr<Player> upPlayer;
}

#endif  /* PLAYER_H */
