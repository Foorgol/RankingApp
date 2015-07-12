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

    ERR err;
    err = pm.enablePlayer(*pl, y, m, d);
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

    // arbitrarily chosen time for old matches: 12:00:00,
    // incremented by one second for each match on the same day.
    // No handling of overflows here (read: not more than 60 matches on the same day)
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
    cvc.addDateTimeCol(MA_TIMESTAMP, oldMatchTime.get());
    cvc.addDateTimeCol(MA_MATCH_STORED_TIMESTAMP, oldMatchTime.get());
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
    int timestamp;
    int score;
    int playerRef;
    int matchRef;
  };
  vector<_Score> scoreList;

  // step 1: create score events for "initial scores"
  //
  // get player entry dates in chronological order
  DbTab* srcPlayerTab = srcDb->getTab("ranking_player");
  upSqlStatement srcRows = srcDb->execContentQuery("SELECT id FROM ranking_player ORDER BY entryDate ASC, id ASC");
  string prevEntryDate = "42";
  time_t scoreTimestamp = -1;
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

    // reset the timestamp if we have a new day
    string isoDate = r["entryDate"];
    if (isoDate != prevEntryDate)
    {
      upLocalTimestamp t = LocalTimestamp::fromISODate(isoDate, 2, 0, 0);
      scoreTimestamp = t->getRawTime();
    }

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
      _sc.timestamp = scoreTimestamp;
      _sc.playerRef = playerId;

      scoreList.push_back(_sc);
      ++scoreTimestamp;
    }

    prevEntryDate = isoDate;
    srcRows->step();
  }

  // step 2: import old score events
  DbTab* srcScoreTab = srcDb->getTab("ranking_scoring");
  srcRows = srcDb->execContentQuery("SELECT id FROM ranking_scoring ORDER BY scoringDate ASC, reason ASC");
  prevEntryDate = "42";
  scoreTimestamp = -1;
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

    // reset the timestamp if we have a new day
    string isoDate = r["scoringDate"];
    if (isoDate != prevEntryDate)
    {
      upLocalTimestamp t = LocalTimestamp::fromISODate(isoDate, 22, 0, 0);
      scoreTimestamp = t->getRawTime();
    }

    // process entry, based on its type
    int reason = r.getInt("reason");
    _Score sc;
    sc.playerRef = r.getInt("player_id");
    sc.score = r.getInt("score");
    if (reason == 1)   // Match
    {
      sc.reason = SC_TYPE_MATCH;
      sc.matchRef = matchId_old2new[r.getInt("match_id")];

      // for matches, use the (unique) match timestamp as the
      // score timestamp
      int maTime;
      isOk = dstDb->execScalarQueryInt("SELECT Timestamp FROM Match WHERE id=" + to_string(sc.matchRef), &maTime);
      if (!isOk)
      {
        return 404;
      }
      sc.timestamp = maTime;
    }
    if (reason > 1)   // penalty points for lazy players
    {
      sc.timestamp = scoreTimestamp;
      ++scoreTimestamp;

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
    }

    scoreList.push_back(sc);

    prevEntryDate = isoDate;
    srcRows->step();
  }

  // step 3: sort all score events by time
  //sort(scoreList.begin(), scoreList.end(), [](_Score a, _Score b) {
  //  return a.timestamp < b.timestamp;
  //});

  // add all entries to the new score events table
  DbTab* dstTab = dstDb->getTab(TAB_SCORE);
  for (_Score sc : scoreList)
  {
    ColumnValueClause cvc;
    cvc.addIntCol(SC_PLAYER_REF, sc.playerRef);
    cvc.addIntCol(SC_SCORE, sc.score);
    cvc.addIntCol(SC_TIMESTAMP, sc.timestamp);
    cvc.addIntCol(SC_TYPE, sc.reason);
    cvc.addIntCol(SC_SCORE_TARGET, SC_SCORE_TARGET_SINGLES);
    if (sc.reason == SC_TYPE_MATCH)
    {
      cvc.addIntCol(SC_MATCH_REF, sc.matchRef);
    }

    dstTab->insertRow(cvc);
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
