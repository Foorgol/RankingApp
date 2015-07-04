#include <iostream>

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


//----------------------------------------------------------------------------


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
