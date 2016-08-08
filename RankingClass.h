#ifndef RANKING_CLASS_H
#define RANKING_CLASS_H

#include <memory>
#include <string>
#include <vector>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericDatabaseObject.h"
#include "GenericObjectManager.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class RankingClass : public GenericDatabaseObject<RankingDb>
  {
    //friend class PlayerMngr;
    friend class SqliteOverlay::GenericObjectManager<RankingDb>;

  public:
    string getName() const;
    bool isSingles() const;

  private:
    RankingClass(RankingDb* db, int rowId);
    RankingClass(RankingDb* db, TabRow row);
  };

  typedef unique_ptr<RankingClass> upRankingClass;
  typedef vector<RankingClass> RankingClassList;
}

#endif  /* RANKING_CLASS_H */
