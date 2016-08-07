
#include "Match.h"
#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"
#include "TabRow.h"

using namespace RankingApp;


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

MATCH_STATE Match::getState() const
{
  int state = row.getInt(MA_STATE);

  return intToMatchState(state);
}

//----------------------------------------------------------------------------

int Match::MatchStateToInt(MATCH_STATE state)
{
  if (state == MATCH_STATE::STAGED)
  {
    return MA_STATE_STAGED;
  }
  if (state == MATCH_STATE::CONFIRMED)
  {
    return MA_STATE_CONFIRMED;
  }

  throw runtime_error("Encountered invalid match state");
}

//----------------------------------------------------------------------------

MATCH_STATE Match::intToMatchState(int state)
{
  if (state == MA_STATE_STAGED)
  {
    return MATCH_STATE::STAGED;
  }
  if (state == MA_STATE_CONFIRMED)
  {
    return MATCH_STATE::CONFIRMED;
  }

  throw runtime_error("Encountered invalid match state");
}

//----------------------------------------------------------------------------

bool Match::isDoubles() const
{
  auto winner2 = row.getInt2(MA_WINNER2_REF);
  return (!(winner2->isNull()));
}

//----------------------------------------------------------------------------

LocalTimestamp Match::getMatchTime() const
{
  //
  // THIS FUNCTION NEEDS TO BE KILLED OR RE-IMPLEMENTED!!
  //
  return LocalTimestamp(nullptr);  // FIX ME: replace nullptr with real time zone

  //return row.getLocalTime(MA_TIMESTAMP);
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
