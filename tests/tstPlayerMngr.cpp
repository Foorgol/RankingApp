#include <string>
#include <gtest/gtest.h>

#include "RankingSystem.h"
#include "BasicTestClass.h"
#include "PlayerMngr.h"
#include "Player.h"
#include "RankingDataDefs.h"
#include "RankingErrCodes.h"

using namespace std;
using namespace RankingApp;

TEST_F(BasicTestFixture, PlayerMngr_createNewPlayer)
{
  upRankingSystem rs = getEmptyRankingSys();
  upRankingDb db = getDirectDatabaseHandle();
  PlayerMngr pm = rs->getPlayerMngr();

  // test empty names
  ERR e;
  upPlayer p = pm.createNewPlayer("", "skdfhsdf", &e);
  ASSERT_EQ(nullptr, p);
  ASSERT_EQ(ERR::INVALID_NAME, e);
  p = pm.createNewPlayer("sdfsfs", "", &e);
  ASSERT_EQ(nullptr, p);
  ASSERT_EQ(ERR::INVALID_NAME, e);
  p = pm.createNewPlayer(" ", "\n", &e);
  ASSERT_EQ(nullptr, p);
  ASSERT_EQ(ERR::INVALID_NAME, e);

  // actually create a player
  DbTab* playerTab = db->getTab(TAB_PLAYER);
  ASSERT_EQ(0, playerTab->length());
  p = pm.createNewPlayer("first", "   last  ", &e);
  ASSERT_TRUE(p != nullptr);
  ASSERT_EQ(ERR::SUCCESS, e);
  ASSERT_EQ(1, playerTab->length());

  // check the entries
  TabRow r = playerTab->operator [](1);
  ASSERT_EQ("first", r[PL_FIRSTNAME]);
  ASSERT_EQ("last", r[PL_LASTNAME]);

  // prevent two players of the same name
  p = pm.createNewPlayer("first", "last", &e);
  ASSERT_EQ(nullptr, p);
  ASSERT_EQ(ERR::PLAYER_EXISTS, e);
}

//----------------------------------------------------------------------------

TEST_F(BasicTestFixture, PlayerMngr_enablePlayer)
{
  upRankingSystem rs = getEmptyRankingSys();
  upRankingDb db = getDirectDatabaseHandle();
  PlayerMngr pm = rs->getPlayerMngr();

  // create a dummy player
  auto pl = pm.createNewPlayer("f", "l");
  ASSERT_TRUE(pl != nullptr);

  // enable the player
  ERR e;
  e = pm.enablePlayer(*pl, 2000, 03, 01);
  ASSERT_EQ(ERR::SUCCESS, e);

  // try to enable the player twice
  e = pm.enablePlayer(*pl, 2000, 03, 01);
  ASSERT_EQ(ERR::PLAYER_IS_ALREADY_ENABLED, e);
  e = pm.enablePlayer(*pl, 1950, 03, 01);
  ASSERT_EQ(ERR::PLAYER_IS_ALREADY_ENABLED, e);
  e = pm.enablePlayer(*pl, 2010, 03, 01);
  ASSERT_EQ(ERR::PLAYER_IS_ALREADY_ENABLED, e);

  // fake a disabling of the player
  DbTab* valTab = db->getTab(TAB_VALIDITY);
  TabRow r = valTab->operator [](1);
  LocalTimestamp fakeEnd{2000, 03, 05, 23, 59, 59};
  r.update(VA_PERIOD_END, fakeEnd);

  // try to enable the player before the the end of
  // the last period
  e = pm.enablePlayer(*pl, 2000, 03, 01);
  ASSERT_EQ(ERR::START_DATE_TOO_EARLY, e);
  e = pm.enablePlayer(*pl, 2000, 03, 05);
  ASSERT_EQ(ERR::START_DATE_TOO_EARLY, e);

  // enable the player after the last period
  e = pm.enablePlayer(*pl, 2000, 03, 06);
  ASSERT_EQ(ERR::SUCCESS, e);

}

//----------------------------------------------------------------------------

TEST_F(BasicTestFixture, PlayerMngr_disablePlayer)
{
  upRankingSystem rs = getEmptyRankingSys();
  upRankingDb db = getDirectDatabaseHandle();
  PlayerMngr pm = rs->getPlayerMngr();

  // create a dummy player
  auto pl = pm.createNewPlayer("f", "l");
  ASSERT_TRUE(pl != nullptr);

  // try to disable a not-enabled player
  ERR e = pm.disablePlayer(*pl, 2000, 03, 05);
  ASSERT_EQ(ERR::PLAYER_IS_NOT_ENABLED, e);

  // enable the player
  e = pm.enablePlayer(*pl, 2000, 03, 03);
  ASSERT_EQ(ERR::SUCCESS, e);

  // try to disable with an invalid date
  e = pm.disablePlayer(*pl, 2000, 03, 02);
  ASSERT_EQ(ERR::END_DATE_TOO_EARLY, e);

  // disabling on the start day should work,
  // actually yielding a one-day membership...
  e = pm.disablePlayer(*pl, 2000, 03, 03);
  ASSERT_EQ(ERR::SUCCESS, e);

  // try to disable a not-enabled player
  e = pm.disablePlayer(*pl, 2000, 03, 05);
  ASSERT_EQ(ERR::PLAYER_IS_NOT_ENABLED, e);
}

//----------------------------------------------------------------------------

TEST_F(BasicTestFixture, PlayerMngr_isPlayerEnabled)
{
  upRankingSystem rs = getEmptyRankingSys();
  upRankingDb db = getDirectDatabaseHandle();
  PlayerMngr pm = rs->getPlayerMngr();

  // create a dummy player
  auto pl = pm.createNewPlayer("f", "l");
  ASSERT_TRUE(pl != nullptr);

  // check with an empty validity table
  ASSERT_FALSE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 3, 5));

  // enable the player
  ERR e = pm.enablePlayer(*pl, 2000, 03, 03);
  ASSERT_EQ(ERR::SUCCESS, e);

  // test an open validity interval
  ASSERT_FALSE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 3, 2));
  ASSERT_TRUE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 3, 3));
  ASSERT_TRUE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 3, 4));

  // disabling on the start day should work,
  // actually yielding a one-day membership...
  e = pm.disablePlayer(*pl, 2000, 03, 03);
  ASSERT_EQ(ERR::SUCCESS, e);

  // test a closed validity interval
  ASSERT_FALSE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 3, 2));
  ASSERT_TRUE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 3, 3));
  ASSERT_FALSE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 3, 4));

  // add another interval so that we can check multiple intervals
  e = pm.enablePlayer(*pl, 2000, 05, 03);
  ASSERT_EQ(ERR::SUCCESS, e);
  e = pm.disablePlayer(*pl, 2000, 05, 13);
  ASSERT_EQ(ERR::SUCCESS, e);

  // test multiple intervals
  ASSERT_FALSE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 3, 2));
  ASSERT_TRUE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 3, 3));
  ASSERT_FALSE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 3, 4));
  ASSERT_FALSE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 4, 14));
  ASSERT_FALSE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 5, 2));
  ASSERT_TRUE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 5, 3));
  ASSERT_TRUE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 5, 13));
  ASSERT_FALSE(pm.isPlayerEnabledOnSpecificDate(*pl, 2000, 5, 14));
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

