
#include "PlayerMngr.h"
#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"

using namespace RankingApp;


PlayerMngr::PlayerMngr(RankingDb* _db)
  :GenericObjectManager(_db), db(_db)
{
  if (_db == nullptr)
  {
    throw std::invalid_argument("Received nullptr as database handle");
  }

  playerTab = db->getTab(TAB_PLAYER);
  validityTab = db->getTab(TAB_VALIDITY);
}

//----------------------------------------------------------------------------

upPlayer PlayerMngr::createNewPlayer(const string& firstName, const string& lastName, ERR* err) const
{
  string fn = firstName;
  ConvenienceFuncs::trim(fn);
  string ln = lastName;
  ConvenienceFuncs::trim(ln);

  // make sure the player names are not empty
  if ((fn.empty()) || (ln.empty()))
  {
    ConvenienceFuncs::setErr(err, ERR::INVALID_NAME);
    return nullptr;
  }

  // check if a player of that name already exists
  auto pl = getPlayerByName(fn, ln);
  if (pl != nullptr)
  {
    ConvenienceFuncs::setErr(err, ERR::PLAYER_EXISTS);
    return nullptr;
  }

  // create the player
  ColumnValueClause cvc;
  cvc.addStringCol(PL_FIRSTNAME, fn);
  cvc.addStringCol(PL_LASTNAME, ln);
  int newId = playerTab->insertRow(cvc);
  if (newId < 1)
  {
    ConvenienceFuncs::setErr(err, ERR::DATABASE_ERROR);
    return nullptr;
  }

  ConvenienceFuncs::setErr(err, ERR::SUCCESS);
  return getPlayerById(newId);
}

//----------------------------------------------------------------------------

upPlayer PlayerMngr::getPlayerByName(const string& firstName, const string& lastName) const
{
  WhereClause w;
  w.addStringCol(PL_FIRSTNAME, firstName);
  w.addStringCol(PL_LASTNAME, lastName);
  return getSingleObjectByWhereClause<Player>(*playerTab, w);
}

//----------------------------------------------------------------------------

upPlayer PlayerMngr::getPlayerById(int id) const
{
  return getSingleObjectByColumnValue<Player>(*playerTab, "id", id);
}

//----------------------------------------------------------------------------

ERR PlayerMngr::enablePlayer(const Player& p, int startYear, int startMonth, int startDay) const
{
  // check 1: make sure that all existing time periods are closed
  // before we open a new one
  WhereClause w;
  w.addIntCol(VA_PLAYER_REF, p.getId());
  w.addNullCol(VA_PERIOD_END);
  if (validityTab->getMatchCountForWhereClause(w) != 0)
  {
    return ERR::PLAYER_IS_ALREADY_ENABLED;
  }

  // construct a timestamp for the beginning of the new time period.
  //
  // Per definition, periods start at 00:00:01 of the selected day
  LocalTimestamp startTime{startYear, startMonth, startDay, 0, 0, 1};

  // check 2: make sure that the new start date is later than all
  // existing validity periods
  auto allPeriods = getObjectsByColumnValue<ValidityPeriod>(*validityTab, VA_PLAYER_REF, p.getId());
  for (ValidityPeriod vp : allPeriods)
  {
    if (vp.determineRelationToPeriod(startTime) != ValidityPeriod::IS_AFTER_PERIOD)
    {
      return ERR::START_DATE_TOO_EARLY;
    }
  }

  // everything is okay and we can setup the new period
  ColumnValueClause cvc;
  cvc.addIntCol(VA_PLAYER_REF, p.getId());
  cvc.addDateTimeCol(VA_PERIOD_START, &startTime);
  validityTab->insertRow(cvc);

  return ERR::SUCCESS;
}

//----------------------------------------------------------------------------

ERR PlayerMngr::disablePlayer(const Player& p, int endYear, int endMonth, int endDay) const
{
  // step 1: get the currently open period
  WhereClause w;
  w.addIntCol(VA_PLAYER_REF, p.getId());
  w.addNullCol(VA_PERIOD_END);
  if (validityTab->getMatchCountForWhereClause(w) == 0)
  {
    return ERR::PLAYER_IS_NOT_ENABLED;
  }

  auto vp = getSingleObjectByWhereClause<ValidityPeriod>(*validityTab, w);
  if (vp == nullptr)
  {
    return ERR::PLAYER_IS_NOT_ENABLED;   // shouldn't occur after the prev. check
  }

  // construct a timestamp for the end of the time period.
  //
  // Per definition, periods end at 23:59:59 of the selected day
  LocalTimestamp endTime{endYear, endMonth, endDay, 23, 59, 59};

  // step 2: make sure the end date is okay
  if (vp->determineRelationToPeriod(endTime) == ValidityPeriod::IS_BEFORE_PERIOD)
  {
    return ERR::END_DATE_TOO_EARLY;
  }

  // everything is okay, we can disable the player
  vp->row.update(VA_PERIOD_END, endTime);

  return ERR::SUCCESS;
}

//----------------------------------------------------------------------------

bool PlayerMngr::isPlayerEnabledOnSpecificDate(const Player& p, int year, int month, int day) const
{
  // construct a timestamp for the middle of the selected day
  LocalTimestamp queriedDate{year, month, day, 12, 0, 0};

  // check if this timestamp is part of any validity period
  auto allPeriods = getObjectsByColumnValue<ValidityPeriod>(*validityTab, VA_PLAYER_REF, p.getId());
  for (ValidityPeriod vp : allPeriods)
  {
    if (vp.isInPeriod(queriedDate))
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------

vector<ValidityPeriod> PlayerMngr::getValidityPeriodsForPlayer(const Player& p) const
{
  return getObjectsByColumnValue<ValidityPeriod>(*validityTab, VA_PLAYER_REF, p.getId());
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

