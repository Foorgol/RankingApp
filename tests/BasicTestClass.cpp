#include <boost/filesystem.hpp>

#include "BasicTestClass.h"
#include "Logger.h"

namespace bfs = boost::filesystem;

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
