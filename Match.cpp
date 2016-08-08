
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

  throw std::runtime_error("Encountered invalid match state");
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

  throw std::runtime_error("Encountered invalid match state");
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
