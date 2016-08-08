#ifndef RANKING_CLASS_MNGR_H
#define RANKING_CLASS_MNGR_H

#include <memory>
#include <vector>

#include <boost/date_time/gregorian/gregorian.hpp>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericObjectManager.h"

using namespace SqliteOverlay;
using namespace boost::gregorian;

namespace RankingApp {

  class RankingClass;

  class RankingClassMngr : public GenericObjectManager<RankingDb>
  {
  public:
    RankingClassMngr(RankingDb* _db);

    // create
    unique_ptr<RankingClass> createNewRankingClass(const string& name, bool isSingles, ERR* err=nullptr) const;

    // getters
    unique_ptr<RankingClass> getRankingClassById(int id) const;
    unique_ptr<RankingClass> getRankingClassByName(const string& name) const;

  private:

  };
}

#endif  /* RANKING_CLASS_MNGR_H */
