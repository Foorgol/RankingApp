#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

#include "BasicTestClass.h"
#include "Logger.h"

#include "RankingSystem.h"
#include "PlayerMngr.h"

namespace bfs = boost::filesystem;
using namespace RankingApp;

constexpr char BasicTestFixture::DB_TEST_FILE_NAME[];

void BasicTestFixture::SetUp()
{
  //qDebug() << "\n\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n";

  log = unique_ptr<SqliteOverlay::Logger>(new SqliteOverlay::Logger("UnitTest"));

  // create a dir for temporary files created during testing
  tstDirPath = boostfs::temp_directory_path();
  if (!(boostfs::exists(tstDirPath)))
  {
    throw std::runtime_error("Could not create temporary directory for test files!");
  }

  log->info("Using directory " + tstDirPath.native() + " for temporary files");
}

//----------------------------------------------------------------------------

void BasicTestFixture::TearDown()
{
  // delete the test database, if still existing
  string dbFileName = getTestDatabaseName();
  bfs::path dbPathObj(dbFileName);
  if (bfs::exists(dbPathObj))
  {
    ASSERT_TRUE(bfs::remove(dbPathObj));
  }
  ASSERT_FALSE(bfs::exists(dbPathObj));
}

//----------------------------------------------------------------------------

string BasicTestFixture::getTestDir() const
{
  return tstDirPath.native();
}

//----------------------------------------------------------------------------

string BasicTestFixture::genTestFilePath(string fName) const
{
  boostfs::path p = tstDirPath;
  p /= fName;
  return p.native();
}

//----------------------------------------------------------------------------

string BasicTestFixture::getTestDatabaseName() const
{
  return genTestFilePath(DB_TEST_FILE_NAME);
}

//----------------------------------------------------------------------------

void BasicTestFixture::printStartMsg(string _tcName)
{
  tcName = _tcName;
  //log.info("\n\n----------- Starting test case '" + tcName + "' -----------");
}

//----------------------------------------------------------------------------

void BasicTestFixture::printEndMsg()
{
  //log.info("----------- End test case '" + tcName + "' -----------\n\n");
}

//----------------------------------------------------------------------------

unique_ptr<RankingApp::RankingSystem> BasicTestFixture::getEmptyRankingSys()
{
  //
  // for some weird reason, ASSERT_xxx don't work in this function and
  // result in compiler errors. Thus, I throw exceptions if something
  // goes wrong.
  //

  // the database shall not exist
  string dbFileName = getTestDatabaseName();
  bfs::path dbPathObj(dbFileName);
  if (bfs::exists(dbPathObj))
  {
    throw std::runtime_error("Database not empty at test case start!");
  }

  RankingApp::ERR err;
  auto rs = RankingApp::RankingSystem::createEmpty(dbFileName, &err);
  if ((rs == nullptr) || (err != RankingApp::ERR::SUCCESS))
  {
    throw std::runtime_error("Could not create test database");
  }

  return rs;
}

//----------------------------------------------------------------------------

unique_ptr<RankingApp::RankingDb> BasicTestFixture::getDirectDatabaseHandle()
{
  // the database must exist
  string dbFileName = getTestDatabaseName();
  bfs::path dbPathObj(dbFileName);
  if (!(bfs::exists(dbPathObj)))
  {
    throw std::runtime_error("Database file does not exist!");
  }

  return SqliteDatabase::get<RankingApp::RankingDb>(dbFileName, false);
}

//----------------------------------------------------------------------------

unique_ptr<RankingApp::RankingSystem> BasicTestFixture::getScenario1()
{
  upRankingSystem rs = getEmptyRankingSys();
  PlayerMngr pm = rs->getPlayerMngr();

  // player 1: enabled since 2000-03-01
  ERR err;
  auto pl = pm.createNewPlayer("f", "l1", &err);
  assert(pl != nullptr);
  assert(ERR::SUCCESS == err);
  err = pm.enablePlayer(*pl, 2000, 3, 1);
  assert(ERR::SUCCESS == err);

  // player 2: enabled since 2000-03-01
  pl = pm.createNewPlayer("f", "l2", &err);
  assert(pl != nullptr);
  assert(ERR::SUCCESS == err);
  err = pm.enablePlayer(*pl, 2000, 3, 1);
  assert(ERR::SUCCESS == err);

  // player 3: enabled 2000-03-01 -- 2000-03-15
  pl = pm.createNewPlayer("f", "l3", &err);
  assert(pl != nullptr);
  assert(ERR::SUCCESS == err);
  err = pm.enablePlayer(*pl, 2000, 3, 1);
  assert(ERR::SUCCESS == err);
  err = pm.disablePlayer(*pl, 2000, 3, 15);
  assert(ERR::SUCCESS == err);

  // player 4: enabled 2000-03-01 -- 2000-03-15
  pl = pm.createNewPlayer("f", "l4", &err);
  assert(pl != nullptr);
  assert(ERR::SUCCESS == err);
  err = pm.enablePlayer(*pl, 2000, 3, 1);
  assert(ERR::SUCCESS == err);
  err = pm.disablePlayer(*pl, 2000, 3, 15);
  assert(ERR::SUCCESS == err);

  return rs;
}
