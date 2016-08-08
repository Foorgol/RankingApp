#ifndef SCORE_MNGR_H
#define SCORE_MNGR_H

#include <memory>
#include <vector>

#include <boost/date_time/gregorian/gregorian.hpp>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericObjectManager.h"

using namespace SqliteOverlay;
using namespace boost::gregorian;

namespace RankingApp {

  class ScoreEvent;
  class RankingClass;

  class ScoreMngr : public GenericObjectManager<RankingDb>
  {
  public:
    ScoreMngr(RankingDb* _db);


  private:

  };
}

#endif  /* SCORE_MNGR_H */
