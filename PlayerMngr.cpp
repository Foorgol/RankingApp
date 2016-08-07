#include <boost/algorithm/string.hpp>

#include "PlayerMngr.h"
#include "MatchMngr.h"
#include "Match.h"
#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"
#include "RankingSystem.h"

using namespace RankingApp;


PlayerMngr::PlayerMngr(RankingDb* _db, RankingSystem* _rs)
  :GenericObjectManager(_db, TAB_PLAYER), rs(_rs)
{
  if (_db == nullptr)
  {
    throw std::invalid_argument("Received nullptr as database handle");
  }
  if (_rs == nullptr)
  {
    throw std::invalid_argument("Received nullptr as ranking system handle");
  }

  validityTab = db->getTab(TAB_VALIDITY);
}

//----------------------------------------------------------------------------

upPlayer PlayerMngr::createNewPlayer(const string& firstName, const string& lastName, ERR* err) const
{
  string fn = firstName;
  boost::trim(fn);
  string ln = lastName;
  boost::trim(ln);

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
  int newId = tab->insertRow(cvc);
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
  return getSingleObjectByWhereClause<Player>(w);
}

//----------------------------------------------------------------------------

upPlayer PlayerMngr::getPlayerById(int id) const
{
  return getSingleObjectByColumnValue<Player>("id", id);
}

//----------------------------------------------------------------------------

ERR PlayerMngr::enablePlayer(const Player& p, int startYear, int startMonth, int startDay, bool skipInitialScore) const
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
  LocalTimestamp startTime{startYear, startMonth, startDay, 0, 0, 1, nullptr};  // FIX ME: replace nullptr with real time zone

  // check 2: make sure that the new start date is later than all
  // existing validity periods
  auto allPeriods = getObjectsByColumnValue<ValidityPeriod>(validityTab, VA_PLAYER_REF, p.getId());
  for (ValidityPeriod vp : allPeriods)
  {
    if (vp.determineRelationToPeriod(startTime) != ValidityPeriod::IS_AFTER_PERIOD)
    {
      return ERR::START_DATE_TOO_EARLY;
    }
  }

  // check 3: make sure we don't have too many active players
  PlayerList activePlayers = getActivePlayersOnGivenDate(startTime.getISODate());
  if (activePlayers.size() >= MAX_ACTIVE_PLAYER_COUNT)
  {
    return ERR::TOO_MANY_PLAYERS;
  }

  // determine the initial score
  int iniScoreSingles = -1;
  int iniScoreDoubles = -1;
  if (!skipInitialScore)
  {
    iniScoreSingles = rs->getInitialScoreForNewPlayer(RANKING_CLASS::SINGLES, startYear, startMonth, startDay);
    iniScoreDoubles = rs->getInitialScoreForNewPlayer(RANKING_CLASS::DOUBLES, startYear, startMonth, startDay);
    if ((iniScoreSingles < 0) || (iniScoreDoubles < 0))
    {
      return ERR::COULD_NOT_DETERMINE_INITIAL_SCORE;
    }
  }

  // everything is okay and we can setup the new period
  ColumnValueClause cvc;
  cvc.addIntCol(VA_PLAYER_REF, p.getId());
  cvc.addDateTimeCol(VA_PERIOD_START, &startTime);
  validityTab->insertRow(cvc);

  // push the initial score to the database
  if (!skipInitialScore)
  {
    time_t rawStartTime = startTime.getRawTime();
    DbTab* scoreTab = db->getTab(TAB_SCORE);
    for (int i=0; i < SCORE_QUEUE_DEPTH; ++i)
    {
      // initial score for singles
      cvc.clear();
      cvc.addIntCol(SC_PLAYER_REF, p.getId());
      cvc.addIntCol(SC_SCORE, iniScoreSingles);
      cvc.addStringCol(SC_ISODATE, startTime.getISODate());
      cvc.addIntCol(SC_TYPE, SC_TYPE_INITIAL);
      cvc.addIntCol(SC_SCORE_TARGET, SC_SCORE_TARGET_SINGLES);
      scoreTab->insertRow(cvc);

      // initial score for doubles
      cvc.clear();
      cvc.addIntCol(SC_PLAYER_REF, p.getId());
      cvc.addIntCol(SC_SCORE, iniScoreDoubles);
      cvc.addStringCol(SC_ISODATE, startTime.getISODate());
      cvc.addIntCol(SC_TYPE, SC_TYPE_INITIAL);
      cvc.addIntCol(SC_SCORE_TARGET, SC_SCORE_TARGET_DOUBLES);
      scoreTab->insertRow(cvc);

      // increase timestamp for the next entry
      ++rawStartTime;
    }
  }


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

  auto vp = getSingleObjectByWhereClause<ValidityPeriod>(validityTab, w);
  if (vp == nullptr)
  {
    return ERR::PLAYER_IS_NOT_ENABLED;   // shouldn't occur after the prev. check
  }

  // construct a timestamp for the end of the time period.
  //
  // Per definition, periods end at 23:59:59 of the selected day
  LocalTimestamp endTime{endYear, endMonth, endDay, 23, 59, 59, nullptr}; // FIX ME: replace nullptr with real time zone

  // step 2: make sure the end date is okay
  if (vp->determineRelationToPeriod(endTime) == ValidityPeriod::IS_BEFORE_PERIOD)
  {
    return ERR::END_DATE_TOO_EARLY;
  }

  // step 3: make sure there aren't any matches for this player after the end date
  MatchMngr mm = MatchMngr(db, rs);
  upMatch latestMatchSingles = mm.getLatestMatchForPlayer(p, RANKING_CLASS::SINGLES, false);
  upMatch latestMatchDoubles = mm.getLatestMatchForPlayer(p, RANKING_CLASS::DOUBLES, false);
  if (latestMatchSingles != nullptr)
  {
    LocalTimestamp maDate = latestMatchSingles->getMatchTime();
    if (maDate > endTime)
    {
      return ERR::END_DATE_TOO_EARLY;
    }
  }
  if (latestMatchDoubles != nullptr)
  {
    LocalTimestamp maDate = latestMatchDoubles->getMatchTime();
    if (maDate > endTime)
    {
      return ERR::END_DATE_TOO_EARLY;
    }
  }

  // everything is okay, we can disable the player
  vp->row.update(VA_PERIOD_END, endTime);

  return ERR::SUCCESS;
}

//----------------------------------------------------------------------------

bool PlayerMngr::isPlayerEnabledOnSpecificDate(const Player& p, int year, int month, int day) const
{
  // construct a timestamp for the middle of the selected day
  LocalTimestamp queriedDate{year, month, day, 12, 0, 0, nullptr};  // FIX ME: replace nullptr with real time zone

  // check if this timestamp is part of any validity period
  auto allPeriods = getObjectsByColumnValue<ValidityPeriod>(validityTab, VA_PLAYER_REF, p.getId());
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

bool PlayerMngr::isPlayerEnabledOnSpecificDate(const Player& p, const string& isoDate) const
{
  StringList _isoDate;
  boost::split(_isoDate, isoDate, boost::is_any_of("-"));
  int year = stoi(_isoDate.at(0));
  int month = stoi(_isoDate.at(1));
  int day = stoi(_isoDate.at(2));

  return isPlayerEnabledOnSpecificDate(p, year, month, day);
}

//----------------------------------------------------------------------------

std::function<bool (Player&, Player&)> PlayerMngr::getPlayerSortFunction_byLastName()
{
  return [](Player& p1, Player& p2) {
    // compare last name
    int cmpLast = p1.getLastName().compare(p2.getLastName());
    if (cmpLast < 0) return true;
    if (cmpLast > 0) return false;

    // last name is identical ==> compare first name
    int cmpFirst = p1.getFirstName().compare(p2.getFirstName());
    if (cmpFirst < 0) return true;
    if (cmpFirst > 0) return false;

    // names are identical. So we display the player who has registered earlier
    // as the first player
    if (p1.getId() < p2.getId()) return true;
    return false;
  };
}

//----------------------------------------------------------------------------

vector<ValidityPeriod> PlayerMngr::getValidityPeriodsForPlayer(const Player& p) const
{
  return getObjectsByColumnValue<ValidityPeriod>(validityTab, VA_PLAYER_REF, p.getId());
}

//----------------------------------------------------------------------------

upLocalTimestamp PlayerMngr::getEarliestActivationDateForPlayer(const Player& p) const
{
  WhereClause w;
  w.addIntCol(VA_PLAYER_REF, p.getId());
  w.addNotNullCol(VA_PERIOD_START);
  w.setOrderColumn_Asc(VA_PERIOD_START);
  auto valPer = getSingleObjectByWhereClause<ValidityPeriod>(validityTab, w);
  if (valPer == nullptr) return nullptr;

  return valPer->getPeriodStart();
}

//----------------------------------------------------------------------------

upLocalTimestamp PlayerMngr::getLatestDeactivationDateForPlayer(const Player& p) const
{
  // is there a validity period item with end == NULL?
  WhereClause w;
  w.addIntCol(VA_PLAYER_REF, p.getId());
  w.addNotNullCol(VA_PERIOD_START);
  w.addNullCol(VA_PERIOD_END);
  int cnt = validityTab->getMatchCountForWhereClause(w);
  if (cnt != 0) return nullptr;

  // no "open" validity period. Se we have to search for the latest end date
  w.clear();
  w.addIntCol(VA_PLAYER_REF, p.getId());
  w.addNotNullCol(VA_PERIOD_START);
  w.addNotNullCol(VA_PERIOD_END);
  w.setOrderColumn_Desc(VA_PERIOD_END);
  auto valPer = getSingleObjectByWhereClause<ValidityPeriod>(validityTab, w);
  if (valPer == nullptr) return nullptr;   // this shouldn't happen, but anyway...

  return valPer->getPeriodEnd();
}

//----------------------------------------------------------------------------

PlayerList PlayerMngr::getAllPlayers() const
{
  return getAllObjects<Player>();
}

//----------------------------------------------------------------------------

PlayerList PlayerMngr::getActivePlayersOnGivenDate(const string& isoDate) const
{
  PlayerList result;
  for (Player p : getAllPlayers())
  {
    if (isPlayerEnabledOnSpecificDate(p, isoDate))
    {
      result.push_back(p);
    }
  }

  return result;
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

