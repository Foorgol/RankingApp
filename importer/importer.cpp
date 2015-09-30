#include <iostream>
#include <map>

#include <boost/filesystem.hpp>

#include "RankingDb.h"
#include "RankingSystem.h"
#include "SqliteDatabase.h"
#include "RankingDataDefs.h"
#include "ConvenienceFuncs.h"


using namespace std;
using namespace SqliteOverlay;
using namespace RankingApp;
namespace bfs = boost::filesystem;

class NullDb : public SqliteDatabase
{
public:
  virtual void populateTables() {}
  virtual void populateViews() {}

  NullDb() : SqliteDatabase() {}
  NullDb(string sqliteFilename, bool createNew) : SqliteDatabase(sqliteFilename, createNew) {}
};

//----------------------------------------------------------------------------

int importPlayers(unique_ptr<NullDb>& srcDb, unique_ptr<RankingDb>& dstDb, upRankingSystem& rs)
{
  PlayerMngr pm = rs->getPlayerMngr();

  DbTab* srcTab = srcDb->getTab("ranking_player");
  WhereClause where;
  where.addIntCol("id", ">", 0);
  where.setOrderColumn_Asc("id");
  DbTab::CachingRowIterator it = srcTab->getRowsByWhereClause(where);
  while (!(it.isEnd()))
  {
    TabRow r = *it;

    string l = r["lastname"];
    string f = r["firstname"];
    int oldId = r.getInt("id");

    ERR err;
    upPlayer pl = pm.createNewPlayer(f, l, &err);
    if ((pl == nullptr) || (err != ERR::SUCCESS))
    {
      return 100;
    }

    if (pl->getId() != oldId)
    {
      return 101;
    }

    ++it;
  }

  return 0;
}

//----------------------------------------------------------------------------

int importValidityDates(unique_ptr<NullDb>& srcDb, unique_ptr<RankingDb>& dstDb, upRankingSystem& rs)
{
  PlayerMngr pm = rs->getPlayerMngr();

  DbTab* srcTab = srcDb->getTab("ranking_player");
  DbTab::CachingRowIterator it = srcTab->getAllRows();

  while (!(it.isEnd()))
  {
    TabRow r = *it;

    int id = r.getInt("id");
    auto pl = pm.getPlayerById(id);

    string entry = r["entryDate"];
    StringList chunks = ConvenienceFuncs::splitString(entry, '-');
    int y = stoi(chunks[0]);
    int m = stoi(chunks[1]);
    int d = stoi(chunks[2]);

    // re-write wrong entry dates of some players to make database
    // consistent again
    if (id == 65)
    {
      --d;
    }
    if (id == 76)
    {
      d = d - 12;
    }

    ERR err;
    err = pm.enablePlayer(*pl, y, m, d, true);
    if (err != ERR::SUCCESS)
    {
      return 200;
    }

    auto exit = r.getString2("exitDate");
    if (!(exit->isNull()))
    {
      string exitDate = exit->get();
      ConvenienceFuncs::trim(exitDate);
      if (!(exitDate.empty()))
      {
        chunks = ConvenienceFuncs::splitString(exit->get(), '-');
        y = stoi(chunks[0]);
        m = stoi(chunks[1]);
        d = stoi(chunks[2]);

        err = pm.disablePlayer(*pl, y, m, d);
        if (err != ERR::SUCCESS)
        {
          return 201;
        }
      }
    }

    ++it;
  }

  return 0;
}

//----------------------------------------------------------------------------

int importMatches(unique_ptr<NullDb>& srcDb, unique_ptr<RankingDb>& dstDb, upRankingSystem& rs, map<int, int>& matchId_old2new)
{
  DbTab* srcTab = srcDb->getTab("ranking_match");
  DbTab* dstTab = dstDb->getTab(TAB_MATCH);
  matchId_old2new.clear();

  // get matches in chronological order
  upSqlStatement srcRows = srcDb->execContentQuery("SELECT id FROM ranking_match ORDER BY matchDate ASC, id ASC");
  int sameDayMatchCount = 0;
  string prevMatchDate = "42";
  while (srcRows->hasData())
  {
    bool isOk;

    int oldId;
    isOk = srcRows->getInt(0, &oldId);
    if (!isOk)
    {
      return 301;
    }

    TabRow r = srcTab->operator [](oldId);

    string isoDate = r["matchDate"];
    if (isoDate != prevMatchDate)
    {
      sameDayMatchCount = 0;
    }

    // try to convert the isoDate into a time object. This is only
    // for checking that the contents of "isoDate" are valid
    upLocalTimestamp oldMatchTime = LocalTimestamp::fromISODate(isoDate, 12, 0, sameDayMatchCount);
    if (oldMatchTime == nullptr)
    {
      return 302;
    }

    // create a new database entry for this match
    int winnerId = r.getInt("winner_id");
    int loserId = r.getInt("loser_id");
    int _threeGames = r.getInt("isThreeGames");
    bool isThreeGames = (_threeGames == 1);   // for very old matches, a "2" indicates two games
    ColumnValueClause cvc;
    cvc.addIntCol(MA_WINNER1_REF, winnerId);
    cvc.addIntCol(MA_LOSER1_REF, loserId);
    cvc.addIntCol(MA_RESULT, isThreeGames ? 1 : 0);
    cvc.addIntCol(MA_STATE, MA_STATE_CONFIRMED);
    cvc.addStringCol(MA_ISODATE, isoDate);
    cvc.addDateTimeCol(MA_MATCH_STORED_TIMESTAMP, oldMatchTime.get());
    cvc.addDateTimeCol(MA_MATCH_CONFIRMED_TIMESTAMP, oldMatchTime.get());
    int newId = dstTab->insertRow(cvc);
    if (newId < 1)
    {
      return 303;
    }

    // map the old match id to the new one
    matchId_old2new[oldId] = newId;

    ++sameDayMatchCount;
    prevMatchDate = isoDate;
    srcRows->step();
  }

  return 0;
}

//----------------------------------------------------------------------------

int importScores(unique_ptr<NullDb>& srcDb, unique_ptr<RankingDb>& dstDb, upRankingSystem& rs, map<int, int>& matchId_old2new)
{
  // a helper struct for scoring data
  class _Score
  {
  public:
    int reason;
    string isoDate;
    int seqNum;
    int score;
    int playerRef;
    int matchRef;
  };
  vector<_Score> scoreList;

  int seqNum = 0;

  // step 1: create score events for "initial scores"
  //
  // get player entry dates in chronological order
  DbTab* srcPlayerTab = srcDb->getTab("ranking_player");
  PlayerMngr pm = rs->getPlayerMngr();
  upSqlStatement srcRows = srcDb->execContentQuery("SELECT id FROM ranking_player ORDER BY entryDate ASC, id ASC");
  while (srcRows->hasData())
  {
    bool isOk;
    int playerId;
    isOk = srcRows->getInt(0, &playerId);
    if (!isOk)
    {
      return 401;
    }

    TabRow r = srcPlayerTab->operator [](playerId);

    // get the entry date for that player
    //
    // use the entry in the target database, not the
    // entry in the source database, because when importing
    // the players, I had to re-write some entry dates because
    // the old database is inconsistent
    auto pl = pm.getPlayerById(playerId);
    auto earliestActivation = pm.getEarliestActivationDateForPlayer(*pl);
    string isoDate = earliestActivation->getISODate();

    // split the initial score into five separate score events
    StringList iniScore = ConvenienceFuncs::splitString(r["initialScore"], ',');
    if (iniScore.size() != 5)
    {
      return 402;
    }
    for (string sScore : iniScore)
    {
      int sc = stoi(sScore);
      _Score _sc;
      _sc.reason = SC_TYPE_INITIAL;
      _sc.score = sc;
      _sc.isoDate = isoDate;
      _sc.playerRef = playerId;
      _sc.seqNum = seqNum;   // use the same sequence number for all five score events

      scoreList.push_back(_sc);
    }

    ++seqNum;
    srcRows->step();
  }

  // step 2: import old score events
  DbTab* srcScoreTab = srcDb->getTab("ranking_scoring");
  srcRows = srcDb->execContentQuery("SELECT id FROM ranking_scoring ORDER BY scoringDate ASC, reason ASC");
  string prevEntryDate = "42";
  map<int,int> match2SeqNum;
  while (srcRows->hasData())
  {
    bool isOk;
    int scoreId;
    isOk = srcRows->getInt(0, &scoreId);
    if (!isOk)
    {
      return 403;
    }

    TabRow r = srcScoreTab->operator [](scoreId);

    // reset the sequence counter if we have a new day
    string isoDate = r["scoringDate"];
    if (isoDate != prevEntryDate)
    {
      match2SeqNum.clear();
    }

    // process entry, based on its type
    int reason = r.getInt("reason");
    _Score sc;
    sc.playerRef = r.getInt("player_id");
    sc.score = r.getInt("score");
    sc.isoDate = isoDate;

    if (reason == 1)   // Match
    {
      sc.reason = SC_TYPE_MATCH;
      sc.matchRef = matchId_old2new[r.getInt("match_id")];

      // lookup the sequence number for this match, if it
      // has already been assigned. If it has not been
      // assigned, assign a new one
      if (match2SeqNum.find(sc.matchRef) != match2SeqNum.end())
      {
        sc.seqNum = match2SeqNum[sc.matchRef];
      } else {
        sc.seqNum = seqNum;
        match2SeqNum[sc.matchRef] = seqNum;
        ++seqNum;
      }
    }

    if (reason > 1)   // penalty points for lazy players
    {
      switch (reason)
      {
      case 2:
        sc.reason = SC_TYPE_LAZYNESS;
        break;
      case 3:
        sc.reason = SC_TYPE_IUM;
        break;
      default:
        sc.reason = SC_TYPE_OTHER;
      }

      sc.seqNum = seqNum;
      ++seqNum;
    }

    scoreList.push_back(sc);

    prevEntryDate = isoDate;
    srcRows->step();
  }

  // step 3: sort all score events by time and priority
  sort(scoreList.begin(), scoreList.end(), [](_Score a, _Score b) {
    // return (a.seqNum < b.seqNum);

    // first criterion: the score day
    if (a.isoDate < b.isoDate) return true;
    if (a.isoDate > b.isoDate) return false;

    // if scores are on the same day, initial scores come first,
    // then matches, then penalty scores
    if (a.reason < b.reason) return true;
    if (a.reason > b.reason) return false;

    // if we make it to this point, we have the same date
    // and the same scoring type
    // ==> we go by the previously defined sequence number
    return (a.seqNum < b.seqNum);
  });

  // add all entries to the new score events table
  // and re-write the sequence number
  seqNum = -1;
  int prevOldSeqNum = -1;
  DbTab* dstTab = dstDb->getTab(TAB_SCORE);
  for (_Score sc : scoreList)
  {
    // copy the contents from the data structure to the CVC
    ColumnValueClause cvc;
    cvc.addIntCol(SC_PLAYER_REF, sc.playerRef);
    cvc.addIntCol(SC_SCORE, sc.score);
    cvc.addStringCol(SC_ISODATE, sc.isoDate);
    cvc.addIntCol(SC_TYPE, sc.reason);
    cvc.addIntCol(SC_SCORE_TARGET, SC_SCORE_TARGET_SINGLES);
    if (sc.reason == SC_TYPE_MATCH)
    {
      cvc.addIntCol(SC_MATCH_REF, sc.matchRef);
    }

    // re-write the sequence number and make sure that
    // two or more entries with the same consecutive
    // (old) sequence number get the same new sequence number
    int oldSeqNum = sc.seqNum;
    if (prevOldSeqNum != oldSeqNum)
    {
      ++seqNum;
    }
    cvc.addIntCol(SC_SEQ_NUM, seqNum);
    prevOldSeqNum = oldSeqNum;

    dstTab->insertRow(cvc);
  }

  return 0;
}

//----------------------------------------------------------------------------

int rebuildMatchScores(unique_ptr<NullDb>& srcDb, unique_ptr<RankingDb>& dstDb, upRankingSystem& rs)
{
  dstDb->setLogLevel(2);
  rs->setLogLevel(2);

  // create a new column for the updated score
  string sql = "ALTER TABLE " + string(TAB_SCORE) + " ADD COLUMN OldScore INTEGER";
  dstDb->execNonQuery(sql);
  sql = "ALTER TABLE " + string(TAB_SCORE) + " ADD COLUMN DeltaScore INTEGER";
  dstDb->execNonQuery(sql);

  // the way the importer is written it is guaranteed that
  // scores are stored in chronological order

  // go through the match list and recalculate the winner / loser score for each match
  DbTab* scoreTab = dstDb->getTab(TAB_SCORE);
  DbTab* matchTab = dstDb->getTab(TAB_MATCH);
  DbTab::CachingRowIterator it = matchTab->getAllRows();
  while (!(it.isEnd()))
  {
    TabRow matchRow = *it;
    int matchId = matchRow.getId();

    // find the associated score event, if any
    if (scoreTab->getMatchCountForColumnValue(SC_MATCH_REF, matchId) == 0)
    {
      ++it;
      continue;   // this is an old match without score entry, back from the old times with the Excel sheet
    }

    // get the first score row (i.e., the winner)
    TabRow scoreRow1 = scoreTab->getSingleRowByColumnValue(SC_MATCH_REF, matchId);

    // the second score row (i.e., the loser) is one row after the first
    TabRow scoreRow2 = scoreTab->operator [](scoreRow1.getId() + 1);

    // verify that both rows refer to the same match
    if (scoreRow1.getInt(SC_MATCH_REF) != scoreRow2.getInt(SC_MATCH_REF))
    {
      return 501;
    }

    // calculate the ranking for everything up to but not including
    // the current match
    int scoreSeqNum = scoreRow1.getInt(SC_SEQ_NUM);
    PlainRankingEntryList rel = rs->recalcRanking(RANKING_CLASS::SINGLES, scoreSeqNum - 1);

    // get winner and loser
    int winnerId = matchRow.getInt(MA_WINNER1_REF);
    int loserId = matchRow.getInt(MA_LOSER1_REF);

    // get the score for winner and loser
    PlainRankingEntry re1 = *(find_if(rel.cbegin(), rel.cend(), [winnerId](const PlainRankingEntry& re) {
      return (re.playerRef == winnerId);
    }));
    PlainRankingEntry re2 = *(find_if(rel.cbegin(), rel.cend(), [loserId](const PlainRankingEntry& re) {
      return (re.playerRef == loserId);
    }));
    int winnerScore = (re1.value > re2.value) ? re1.value : re2.value;
    int loserScore = (re1.value > re2.value) ? re2.value : re1.value;

    // update the entries in the score table
    int oldScore = scoreRow1.getInt(SC_SCORE);
    scoreRow1.update("OldScore", oldScore);
    if (scoreRow1.getInt(SC_PLAYER_REF) == winnerId)
    {
      scoreRow1.update(SC_SCORE, winnerScore);
      scoreRow1.update("DeltaScore", winnerScore - oldScore);
    } else {
      scoreRow1.update(SC_SCORE, loserScore);
      scoreRow1.update("DeltaScore", loserScore - oldScore);
    }
    oldScore = scoreRow2.getInt(SC_SCORE);
    scoreRow2.update("OldScore", oldScore);
    if (scoreRow2.getInt(SC_PLAYER_REF) == winnerId)
    {
      scoreRow2.update(SC_SCORE, winnerScore);
      scoreRow2.update("DeltaScore", winnerScore - oldScore);
    } else {
      scoreRow2.update(SC_SCORE, loserScore);
      scoreRow2.update("DeltaScore", loserScore - oldScore);
    }

    // next match
    ++it;

    cerr << int(it.getPercentage() * 100);
  }


  return 0;
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

int doImport(const string& srcDbName, const string& dstDbName)
{
  // open the source database
  auto srcDb = SqliteDatabase::get<NullDb>(srcDbName, false);
  if (srcDb == nullptr)
  {
    cerr << "Could not open " << srcDbName << endl;
    return 2;
  }

  // create and/or delete the destination database
  bfs::path dstDbPath{dstDbName};
  if (bfs::exists(dstDbPath))
  {
    bool result = bfs::remove(dstDbPath);

    if (!result || (bfs::exists(dstDbPath)))
    {
      cerr << "Could not delete " << dstDbName << endl;
      return 3;
    }
  }
  auto dstDb = SqliteDatabase::get<RankingDb>(dstDbName, true);
  if (dstDb == nullptr)
  {
    cerr << "Could create new database " << dstDbName << endl;
    return 4;
  }

  ERR err;
  upRankingSystem rs = RankingSystem::get(dstDbName, &err);
  if ((rs == nullptr) || (err != ERR::SUCCESS))
  {
    cerr << "Could not initialize RankingSystem-object on " << dstDbName << endl;
    return 6;
  }

  // import the player names
  int result = importPlayers(srcDb, dstDb, rs);
  if (result != 0)
  {
    return result;
  }

  // import validity periods
  result = importValidityDates(srcDb, dstDb, rs);
  if (result != 0)
  {
    return result;
  }

  // import matches
  map<int, int> matchId_old2new;
  result = importMatches(srcDb, dstDb, rs, matchId_old2new);
  if (result != 0)
  {
    return result;
  }

  // import scores
  result = importScores(srcDb, dstDb, rs, matchId_old2new);
  if (result != 0)
  {
    return result;
  }

  // rebuild / re-calculate the match scores to fix
  // improper chronological data entries from the past
  result = rebuildMatchScores(srcDb, dstDb, rs);
  if (result != 0)
  {
    return result;
  }

  // recalc the ranking
  rs->recalcRankings();

  return 0;
}

//----------------------------------------------------------------------------

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    cout << "Wrong number of arguments" << endl;
    return 1;
  }

  string infileName = string{argv[1]};
  string outfileName = string{argv[2]};

  cout << "Using infile = " << infileName << endl;
  cout << "Using outfile = " << outfileName << endl;

  return doImport(infileName, outfileName);
}
