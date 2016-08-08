#include <boost/algorithm/string.hpp>

#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"
#include "ScoreMngr.h"
#include "RankingClass.h"

namespace RankingApp
{
  ScoreMngr::ScoreMngr(RankingDb* _db)
    :GenericObjectManager(_db, TAB_SCORE)
  {
    if (_db == nullptr)
    {
      throw std::invalid_argument("Received nullptr as database handle");
    }
  }


  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
}
