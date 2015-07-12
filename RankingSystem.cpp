
#include "RankingSystem.h"
#include "RankingDb.h"
#include "RankingDataDefs.h"
#include "ConvenienceFuncs.h"

using namespace RankingApp;

unique_ptr<RankingApp::RankingSystem> RankingApp::RankingSystem::get(const string& fname, RankingApp::ERR* err)
{
  return doInit(fname, false, err);
}

//----------------------------------------------------------------------------

unique_ptr<RankingSystem> RankingSystem::createEmpty(const string& fname, ERR* err)
{
  return doInit(fname, true, err);
}

//----------------------------------------------------------------------------

bool RankingApp::RankingSystem::isValidFilename(const string& fname)
{
  string fn = fname;
  ConvenienceFuncs::trim(fn);
  if (fn.empty()) return false;
  if (fn.front() == ':') return false;  // avoid special file names such as ":memory:"
  return true;
}

//----------------------------------------------------------------------------

PlayerMngr RankingSystem::getPlayerMngr() const
{
  return PlayerMngr(db.get());
}

//----------------------------------------------------------------------------

MatchMngr RankingSystem::getMatchMngr() const
{
  return MatchMngr{db.get()};
}

//----------------------------------------------------------------------------

void RankingSystem::recalcRankings(int maxYear, int maxMonth, int maxDay) const
{
  // calculate the "sub-rankings"
  PlainRankingEntryList singles = recalcRanking(RANKING_CLASS::SINGLES, maxYear, maxMonth, maxDay);
  PlainRankingEntryList doubles = recalcRanking(RANKING_CLASS::DOUBLES, maxYear, maxMonth, maxDay);

  // replace the old ranking tables completely
  db->dropAndCreateRankingTab();

  // copy the new ranking(s) to the table
  DbTab* rankTab = db->getTab(TAB_RANKING);
  auto storePlainRankingEntry = [&](PlainRankingEntry& re, int rankClass) {
    ColumnValueClause cvc;
    cvc.addIntCol(RA_PLAYER_REF, re.playerRef);
    cvc.addIntCol(RA_RANKING_CLASS, rankClass);
    cvc.addIntCol(RA_RANK, re.rank);
    cvc.addIntCol(RA_VALUE, re.value);
    cvc.addStringCol(RA_SCORE_QUEUE, re.scores.toString());
    rankTab->insertRow(cvc);
  };
  for (PlainRankingEntry& re : singles)
  {
    storePlainRankingEntry(re, RA_RANKING_CLASS_SINGLES);
  }
  for (PlainRankingEntry& re : doubles)
  {
    storePlainRankingEntry(re, RA_RANKING_CLASS_DOUBLES);
  }
}

//----------------------------------------------------------------------------

int RankingSystem::RankingClassToInt(RANKING_CLASS rc) const
{
  if (rc == RANKING_CLASS::SINGLES)
  {
    return RA_RANKING_CLASS_SINGLES;
  }
  if (rc == RANKING_CLASS::DOUBLES)
  {
    return RA_RANKING_CLASS_DOUBLES;
  }

  return -1;
}

//----------------------------------------------------------------------------

RankingSystem::RankingSystem(upRankingDb _db)
{
  // this checked should never trigger, because we only call
  // this function from get() with a pre-checked pointer.
  // But anyway...
  if (_db == nullptr)
  {
    throw std::invalid_argument("Received nullptr for database handle!");
  }

  // store the database handle
  db = std::move(_db);
}

//----------------------------------------------------------------------------

unique_ptr<RankingSystem> RankingSystem::doInit(const string& fname, bool doCreateNew, ERR* err)
{
  if (!(isValidFilename(fname)))
  {
    ConvenienceFuncs::setErr(err, ERR::INVALID_FILENAME);
    return nullptr;
  }

  // try to create a new database or open an existing
  string fn = fname;
  ConvenienceFuncs::trim(fn);
  auto dbPtr = SqliteDatabase::get<RankingDb>(fn, doCreateNew);

  // if that fails, return null
  if (dbPtr == nullptr)
  {
    if (doCreateNew)
    {
      ConvenienceFuncs::setErr(err, ERR::FILE_ALREADY_EXISTS);
    } else {
      ConvenienceFuncs::setErr(err, ERR::FILE_NOT_EXISTING_OR_INVALID);
    }
    return nullptr;
  }

  // create a new instance
  RankingSystem* tmpPtr = new RankingSystem(std::move(dbPtr));
  ConvenienceFuncs::setErr(err, ERR::SUCCESS);
  return unique_ptr<RankingSystem>(tmpPtr);
}

//----------------------------------------------------------------------------

PlainRankingEntryList RankingSystem::recalcRanking(RANKING_CLASS rankClass, int maxYear, int maxMonth, int maxDay) const
{
  DbTab* scoreTab = db->getTab(TAB_SCORE);
  int rClass = RankingClassToInt(rankClass);

  // if we don't have a valid date in maxYear, maxMonth, maxDay we calculate
  // the ranking up to and including the latest entry. Otherwise, we only
  // calc up to and including the given date
  LocalTimestamp maxDate;
  try
  {
    maxDate = LocalTimestamp(maxYear, maxMonth, maxDay, 23, 59, 59);
  } catch (exception e) {
    maxDate = LocalTimestamp();  // "now"
    maxDate = *(LocalTimestamp::fromISODate(maxDate.getISODate(), 23, 59, 59));
  }

  // get a list of the active players
  PlayerMngr pm = getPlayerMngr();
  PlayerList activePlayers = pm.getActivePlayersOnGivenDate(maxDate.getISODate());

  // collect the scores for each active player in chronological order
  PlainRankingEntryList result;
  for (Player p : activePlayers)
  {
    ScoreQueue queue(SCORE_QUEUE_DEPTH);

    WhereClause where;
    where.addIntCol(SC_PLAYER_REF, p.getId());
    where.addIntCol(SC_TIMESTAMP, "<=", maxDate.getRawTime());
    where.addIntCol(SC_SCORE_TARGET, rClass);
    where.setOrderColumn_Asc(SC_TIMESTAMP);
    upSqlStatement allScores = db->execContentQuery(where.getSelectStmt(TAB_SCORE, false));

    while (allScores->hasData())
    {
      int id;
      allScores->getInt(0, &id);
      TabRow r = scoreTab->operator [](id);
      queue.pushScore(r.getInt(SC_SCORE));
      allScores->step();
    }

    PlainRankingEntry re;
    re.playerRef = p.getId();
    re.scores = queue;
    result.push_back(re);
  }

  // sort ranking entries
  sortPlainRankingEntryListInPlace(result, rankClass);


  // assign ranks and values to the sorted ranking entries
  assignRanksAndValuesToSortedPlainRankingEntryListInPlace(result);

  // done!
  return result;
}

//----------------------------------------------------------------------------

void RankingSystem::sortPlainRankingEntryListInPlace(PlainRankingEntryList& rel, const RANKING_CLASS& rankClass) const
{
  sort(rel.begin(), rel.end(), [&](const PlainRankingEntry& a, const PlainRankingEntry& b) {
    int aSum = a.scores.getSum();
    int bSum = b.scores.getSum();
    if (aSum > bSum) return true;
    if (aSum < bSum) return false;

    //
    // score sums are equal
    //

    // first criterion: who played more recently?
    MatchMngr mm = getMatchMngr();
    PlayerMngr pm = getPlayerMngr();
    upPlayer aPlayer = pm.getPlayerById(a.playerRef);
    upPlayer bPlayer = pm.getPlayerById(b.playerRef);
    upMatch aMatch = mm.getLatestMatchForPlayer(*aPlayer, rankClass);
    upMatch bMatch = mm.getLatestMatchForPlayer(*bPlayer, rankClass);
    if ((aMatch == nullptr) && (bMatch != nullptr))
    {
      return false;   // "a" has never played but "b" did ==> b is better than a
    }
    if ((bMatch == nullptr) && (aMatch != nullptr))
    {
      return true;   // "b" has never played but "a" did ==> a is better than b
    }

    //
    // both players have never played
    //

    // second criterion: who joined earlier?
    string sql = "SELECT " + string(VA_PERIOD_START) + " FROM " + string(TAB_VALIDITY);
    sql += " WHERE " + string(VA_PLAYER_REF) + "= @player_id AND " + string(VA_PERIOD_END);
    sql += " IS NULL";
    upSqlStatement stmtA = db->prepStatement(sql);
    upSqlStatement stmtB = db->prepStatement(sql);
    stmtA->bindInt(1, a.playerRef);
    stmtB->bindInt(1, b.playerRef);
    int aTime = 0;
    int bTime = 0;
    db->execScalarQueryInt(stmtA, &aTime);
    db->execScalarQueryInt(stmtB, &bTime);
    if (aTime < bTime)
    {
      return true;   // "a" joined earlier
    }
    if (bTime < aTime)
    {
      return false;   // "b" joined earlier
    }

    //
    // both players joined at the same time
    //

    // last criterion: lower player id goes first
    return (a.playerRef < b.playerRef);
  });

}

//----------------------------------------------------------------------------

void RankingSystem::assignRanksAndValuesToSortedPlainRankingEntryListInPlace(PlainRankingEntryList& rel) const
{
  int lastVal = MAX_RANK_VALUE;
  int rank = 0;
  int lastSum = -1;
  for (PlainRankingEntry& re : rel)
  {
    ++rank;
    re.rank = rank;

    // if this player has the same score sum as the previous one
    // it gets the same value assigned. Otherwise, the value
    // is calculated from the rank and the step size
    if (re.scores.getSum() == lastSum)
    {
      re.value = lastVal;
    } else {
      lastSum = re.scores.getSum();
      lastVal = MAX_RANK_VALUE - (re.rank - 1) * RANK_VALUE_STEP;
      re.value = lastVal;
    }
  }
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
