#ifndef BASICTESTCLASS_H
#define	BASICTESTCLASS_H

#include <string>
#include <memory>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include "Logger.h"
#include "RankingSystem.h"

using namespace std;
namespace boostfs = boost::filesystem;

class EmptyFixture
{

};

class BasicTestFixture : public ::testing::Test
{
public:
  static constexpr char DB_TEST_FILE_NAME[] = "RankingTest.db";

protected:

  virtual void SetUp ();
  virtual void TearDown ();

  string getTestDir () const;
  string genTestFilePath(string fName) const;
  string getTestDatabaseName() const;
  boostfs::path tstDirPath;
  unique_ptr<SqliteOverlay::Logger> log;
  void printStartMsg(string _tcName);
  void printEndMsg();

  unique_ptr<RankingApp::RankingSystem> getEmptyRankingSys();
  unique_ptr<RankingApp::RankingDb> getDirectDatabaseHandle();

  unique_ptr<RankingApp::RankingSystem> getScenario1();

private:
  string tcName;
};

#endif /* BASICTESTCLASS_H */
