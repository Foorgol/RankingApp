
#include "Match.h"
#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"
#include "TabRow.h"

using namespace RankingApp;


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

MatchState Match::getState() const
{
  int state = row.getInt(MA_STATE);

  return static_cast<MatchState>(state);
}

//----------------------------------------------------------------------------

bool Match::isDoubles() const
{
  auto winner2 = row.getInt2(MA_WINNER2_REF);
  return (!(winner2->isNull()));
}

//----------------------------------------------------------------------------

boost::gregorian::date Match::getMatchTime() const
{
  return row.getDate(MA_DATE);
}

//----------------------------------------------------------------------------

Match::Match(RankingDb* db, int rowId)
:GenericDatabaseObject(db, TAB_PLAYER, rowId)
{
}

//----------------------------------------------------------------------------

Match::Match(RankingDb* db, TabRow row)
:GenericDatabaseObject(db, row)
{
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
