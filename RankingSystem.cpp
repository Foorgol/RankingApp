#include <boost/algorithm/string.hpp>


#include "RankingSystem.h"

#include "RankingDb.h"
#include "RankingDataDefs.h"
#include "ConvenienceFuncs.h"
#include "RankingClass.h"

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
  boost::trim(fn);
  if (fn.empty()) return false;
  if (fn.front() == ':') return false;  // avoid special file names such as ":memory:"
  return true;
}

//----------------------------------------------------------------------------

PlayerMngr RankingSystem::getPlayerMngr()
{
  return PlayerMngr(db.get(), this);
}

//----------------------------------------------------------------------------

MatchMngr RankingSystem::getMatchMngr()
{
  return MatchMngr{db.get(), this};
}

//----------------------------------------------------------------------------

void RankingSystem::recalcRankings(int maxSeqNumIncluded)
{
  //
  // FIX ME: THIS NEED TO BE COMPLETELY RE-WRITTEN!
  //
  /*
  // calculate the "sub-rankings"
  PlainRankingEntryList singles = recalcRanking(RankingClass::Singles, maxSeqNumIncluded);
  PlainRankingEntryList doubles = recalcRanking(RankingClass::Doubles, maxSeqNumIncluded);

  // write to the database
  storeRankingEntries(singles, doubles);
  */
}

//----------------------------------------------------------------------------

void RankingSystem::storeRankingEntries(const PlainRankingEntryList& singlesRanking_sorted, const PlainRankingEntryList& doublesRanking_sorted)
{
  //
  // FIX ME: THIS NEEDS TO BE COMPLETELY RE-WRITTEN!
  //

  /*

  // replace the old ranking tables completely
  db->dropAndCreateRankingTab();

  // copy the new ranking(s) to the table
  DbTab* rankTab = db->getTab(TAB_RANKING);
  auto storePlainRankingEntry = [&](const PlainRankingEntry& re, int rankClass) {
    ColumnValueClause cvc;
    cvc.addIntCol(RA_PLAYER_REF, re.playerRef);
    cvc.addIntCol(RA_RANKING_CLASS, rankClass);
    cvc.addIntCol(RA_RANK, re.rank);
    cvc.addIntCol(RA_VALUE, re.value);
    cvc.addStringCol(RA_SCORE_QUEUE, re.scores.toString());
    rankTab->insertRow(cvc);
  };
  for (const PlainRankingEntry& re : singlesRanking_sorted)
  {
    storePlainRankingEntry(re, RA_RANKING_CLASS_SINGLES);
  }
  for (const PlainRankingEntry& re : doublesRanking_sorted)
  {
    storePlainRankingEntry(re, RA_RANKING_CLASS_DOUBLES);
  }

  */
}

//----------------------------------------------------------------------------

ERR RankingSystem::confirmMatchAndUpdateRanking(const Match& ma)
{
  // try to set the match status to "confirmed"
  MatchMngr mm = getMatchMngr();
  ERR err = mm.confirmMatch(ma);
  if (err != ERR::SUCCESS)
  {
    return err;
  }

  // default: score time = match time
  greg::date scoreDate = ma.getMatchTime();

  // is the match date later than the latest score event?
  // if not, we have to recalc all scores / matches that
  // occurred after this match
//  LocalTimestamp latestEvent = getLatestScoreEventTimestamp();
//  if (latestEvent >= scoreTime)
//  {
//    // shift
//  }

  //
  // FIX ME: RE-WRITE COMPLETELY BELOW THIS POINT
  //

  /*
  // get the (now outdated) ranking
  RankingClass rankClass = ma.isDoubles() ? RankingClass::Doubles : RankingClass::Singles;
  PlainRankingEntryList rel = getSortedRanking(rankClass);

  // !!! HIER WEITERMACHEN !!!
  // FRAGE: alle bereits verteilten punkte, die zeitlich nach dem spiel liegen, neu berechnen?
*/
  return ERR::SUCCESS;
}

//----------------------------------------------------------------------------

PlainRankingEntryList RankingSystem::getSortedRanking(const RankingClass& rankClass) const
{
  PlainRankingEntryList result;

  int rClass = RankingClassToInt(rankClass);

  string sql = "SELECT id FROM ? WHERE ?=? ORDER BY ? ASC";
  upSqlStatement stmt = db->prepStatement(sql);
  stmt->bindString(1, TAB_RANKING);
  stmt->bindString(2, RA_RANK_CLASS_REF);
  stmt->bindInt(3, rankClass.getId());
  stmt->bindString(4, RA_RANK);

  db->execContentQuery(stmt);
  DbTab* rankingTab = db->getTab(TAB_RANKING);
  while (stmt->hasData())
  {
    int id;
    stmt->getInt(0, &id);
    TabRow r = rankingTab->operator [](id);

    PlainRankingEntry re;
    re.playerRef = r.getInt(RA_PLAYER_REF);
    re.rank = r.getInt(RA_RANK);
    re.scores = *(ScoreQueue::fromString(ScoreQueueDepth, r[RA_SCORE_QUEUE]));
    re.value = r.getInt(RA_VALUE);

    result.push_back(re);
  }

  return result;
}

//----------------------------------------------------------------------------

int RankingSystem::getInitialScoreForNewPlayer(const RankingClass& rankClass, greg::date startDate)
{
  //
  // THIS FUNCTION NEEDS TO BE COMPLETELY RE-WRITTEN
  //

  return -1;

  /*
  // we can only determine the score if we have no unconfirmed matches before or on the
  // expected start date
  LocalTimestamp startTime = LocalTimestamp(startYear, startMonth, startDay, 0, 0, 1);
  WhereClause w;
  w.addStringCol(MA_ISODATE, "<=", startTime.getISODate());
  w.addIntCol(MA_STATE, MA_STATE_STAGED);
  if (rankClass == RANKING_CLASS::SINGLES)
  {
    w.addNullCol(MA_WINNER2_REF);
  } else {
    w.addNotNullCol(MA_WINNER2_REF);
  }

  DbTab* matchTab = db->getTab(TAB_MATCH);
  if (matchTab->getMatchCountForWhereClause(w) > 0)
  {
    return -1;
  }

  // all scoring before the start date is settled

  // get the ranking up to the start date but without matches
  startTime = LocalTimestamp(startTime.getRawTime() - 1);   // subtract one second fromt the start date
  PlainRankingEntryList rel = recalcRanking(rankClass, startTime.getISODate(), -1);

  // make sure we actually have a ranking
  if (rel.empty())
  {
    return -1;
  }

  // get the value of the worst player
  int worstVal = rel.back().value;
  if (worstVal < (2 * RANK_VALUE_STEP))
  {
    return -1;
  }

  // determine the initial value by subtracting one value step from
  // the worst value
  return (worstVal - RANK_VALUE_STEP);
  */
}

void RankingSystem::setLogLevel(Sloppy::Logger::SeverityLevel newLvl)
{
  db->setLogLevel(newLvl);
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
  boost::trim(fn);
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

PlainRankingEntryList RankingSystem::recalcRanking(const RankingClass& rankClass, int maxSeqNumIncluded)
{
  // get a list of the active players
  PlayerMngr pm = getPlayerMngr();
  greg::date maxDateIncluded;
  maxDateIncluded = getDateForScoreSeqNum(maxSeqNumIncluded);
  if (maxDateIncluded.is_special())  // no scoring entries yet
  {
    db->dropAndCreateRankingTab();
    return PlainRankingEntryList();
  }
  PlayerList activePlayers = pm.getActivePlayersOnGivenDate(maxDateIncluded);

  // collect the scores for each active player in chronological order
  DbTab* scoreTab = db->getTab(TAB_SCORE);
  PlainRankingEntryList result;
  for (Player p : activePlayers)
  {
    ScoreQueue queue(ScoreQueueDepth);

    WhereClause where;
    where.addIntCol(SC_PLAYER_REF, p.getId());
    where.addIntCol(SC_RANK_CLASS_REF, rankClass.getId());
    if (maxSeqNumIncluded >= 0)
    {
      where.addIntCol(SC_SEQ_NUM, "<=", maxSeqNumIncluded);
    }
    where.setOrderColumn_Asc(SC_SEQ_NUM);
    DbTab::CachingRowIterator it = scoreTab->getRowsByWhereClause(where);

    while (!(it.isEnd()))
    {
      queue.pushScore((*it).getInt(SC_SCORE));
      ++it;
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

void RankingSystem::sortPlainRankingEntryListInPlace(PlainRankingEntryList& rel, const RankingClass& rankClass)
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
  int lastVal = MaxRankValue;
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
      lastVal = MaxRankValue - (re.rank - 1) * RankValueStep;
      re.value = lastVal;
    }
  }
}

//----------------------------------------------------------------------------

void RankingSystem::rewriteMatchScores(int maxSeqNumIncluded)
{
  // recalculate the ranking up to one second before minTimeIncluded
  recalcRankings(maxSeqNumIncluded);

  // reprocess all score events dated on maxIsoTimeIncluded and later than maxSeqNumIncluded; if the
  // score event is a match, recalculate the scores
  //for (RankingClass rankclass : {RankingClass::Singles, RankingClass::Singles})
  //{
    // !!! Hier Weitermachen

//    // read the current ranking up to minMinusOne
//    PlainRankingEntryList rel = getSortedRanking(rankclass);

//    // process all score events
//    int rClass = RankingClassToInt(rankclass);
//    WhereClause w;
//    w.addIntCol(SC_SCORE_TARGET, rClass);
//    w.addIntCol(SC_TIMESTAMP, ">=", minTimeIncluded.getRawTime());
//    w.setOrderColumn_Asc(SC_TYPE);
//    DbTab* scoreTab = db->getTab(TAB_SCORE);
//    DbTab::CachingRowIterator it = scoreTab->getRowsByWhereClause(w);
//    while (!(it.isEnd()))
//    {
//    }
//  }

}

//----------------------------------------------------------------------------

greg::date RankingSystem::getDateForScoreSeqNum(int seqNum)
{
  DbTab* scoreTab = db->getTab(TAB_SCORE);

  // if the sequence number is "real" try to find the date
  // for the associated scoring entry
  if (seqNum >= 0)
  {
    if (scoreTab->getMatchCountForColumnValue(SC_SEQ_NUM, seqNum) == 0)
    {
      return greg::date();  // invalid sequence number
    }

    TabRow r = scoreTab->getSingleRowByColumnValue(SC_SEQ_NUM, seqNum);
    return r.getDate(SC_DATE);
  }

  // if the seqNum is less than zero, we determine the LATEST
  // scoring event
  string sql = "SELECT MAX(" + string(SC_DATE) + ") FROM TABLE ";
  sql += string(TAB_SCORE);
  auto maxDate = db->execScalarQueryInt(sql);
  if (maxDate->isNull()) return greg::date();   // no scoring entries yet
  return greg::from_int(maxDate->get());
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
