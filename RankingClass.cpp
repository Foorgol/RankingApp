
#include "RankingClass.h"
#include "RankingDb.h"
#include "RankingDataDefs.h"
#include "TabRow.h"

namespace RankingApp
{

  string RankingClass::getName() const
  {
    return row[RC_NAME];
  }

  //----------------------------------------------------------------------------

  bool RankingClass::isSingles() const
  {
    return (row.getInt(RC_IS_SINGLES) != 0);
  }

  //----------------------------------------------------------------------------

  RankingClass::RankingClass(RankingDb* db, int rowId)
    :GenericDatabaseObject(db, TAB_RANKING_CLASS, rowId)
  {
  }

  //----------------------------------------------------------------------------

  RankingClass::RankingClass(RankingDb* db, TabRow row)
    :GenericDatabaseObject(db, row)
  {
  }

  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------

}
