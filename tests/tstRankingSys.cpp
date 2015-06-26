#include <string>
#include <gtest/gtest.h>

#include "RankingSystem.h"
#include "BasicTestClass.h"

using namespace std;
using namespace RankingApp;

TEST(RankingSys, ValidFilename)
{
  ASSERT_FALSE(RankingSystem::isValidFilename(""));
  ASSERT_FALSE(RankingSystem::isValidFilename("  "));
  ASSERT_FALSE(RankingSystem::isValidFilename("  :memory"));
  ASSERT_FALSE(RankingSystem::isValidFilename(":memory:"));
}

//----------------------------------------------------------------------------

TEST_F(BasicTestFixture, RankingSys_openExistingDb)
{
  // try to open a non-existing file
  ERR e;
  ASSERT_EQ(nullptr, RankingSystem::get("gfdfkghdfg", &e));
  ASSERT_EQ(ERR::FILE_NOT_EXISTING_OR_INVALID, e);

  // try to use an invalid filename
  ASSERT_EQ(nullptr, RankingSystem::get("", &e));
  ASSERT_EQ(ERR::INVALID_FILENAME, e);
  e == ERR::SUCCESS; // reset e
  ASSERT_EQ(nullptr, RankingSystem::get(":memory:", &e));
  ASSERT_EQ(ERR::INVALID_FILENAME, e);

  string fname = getTestDatabaseName();
  // create a new database
  {
    RankingSystem::createEmpty(fname, &e);

    // the database will be closed automatically when leaving this block
  }

  // open the database we've just created
  auto rsys = RankingSystem::createEmpty(fname, &e);
  ASSERT_EQ(e, ERR::SUCCESS);
  ASSERT_TRUE(rsys != nullptr);
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

