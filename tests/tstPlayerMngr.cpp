#include <string>
#include <gtest/gtest.h>

#include "RankingSystem.h"
#include "BasicTestClass.h"
#include "PlayerMngr.h"
#include "Player.h"
#include "RankingDataDefs.h"

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


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

