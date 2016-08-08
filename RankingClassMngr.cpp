#include <boost/algorithm/string.hpp>

#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"
#include "RankingClassMngr.h"
#include "RankingClass.h"

namespace RankingApp
{
  RankingClassMngr::RankingClassMngr(RankingDb* _db)
    :GenericObjectManager(_db, TAB_RANKING_CLASS)
  {
    if (_db == nullptr)
    {
      throw std::invalid_argument("Received nullptr as database handle");
    }
  }

  //----------------------------------------------------------------------------

  unique_ptr<RankingClass> RankingClassMngr::createNewRankingClass(const string& name, bool isSingles, ERR* err) const
  {
    string n = name;
    boost::trim(n);

    // make sure the player names are not empty
    if (n.empty())
    {
      ConvenienceFuncs::setErr(err, ERR::INVALID_NAME);
      return nullptr;
    }

    // check if a ranking class of that name already exists
    auto rc = getRankingClassByName(n);
    if (rc != nullptr)
    {
      ConvenienceFuncs::setErr(err, ERR::OBJECT_EXISTS);
      return nullptr;
    }

    // create the ranking class
    ColumnValueClause cvc;
    cvc.addStringCol(RC_NAME, n);
    cvc.addIntCol(RC_IS_SINGLES, isSingles);
    int newId = tab->insertRow(cvc);
    if (newId < 1)
    {
      ConvenienceFuncs::setErr(err, ERR::DATABASE_ERROR);
      return nullptr;
    }

    ConvenienceFuncs::setErr(err, ERR::SUCCESS);
    return getRankingClassById(newId);
  }

  //----------------------------------------------------------------------------

  unique_ptr<RankingClass> RankingClassMngr::getRankingClassById(int id) const
  {
    return getSingleObjectByColumnValue<RankingClass>("id", id);
  }

  //----------------------------------------------------------------------------

  unique_ptr<RankingClass> RankingClassMngr::getRankingClassByName(const string& name) const
  {
    return getSingleObjectByColumnValue<RankingClass>(RC_NAME, name);
  }

  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
}
