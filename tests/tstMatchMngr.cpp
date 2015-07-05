#include <string>
#include <gtest/gtest.h>

#include "RankingSystem.h"
#include "BasicTestClass.h"
#include "MatchMngr.h"
#include "Match.h"
#include "Player.h"
#include "MatchMngr.h"
#include "RankingDataDefs.h"
#include "RankingErrCodes.h"

using namespace std;
using namespace RankingApp;

TEST_F(BasicTestFixture, PlayerMngr_stageNewMatch_Singles)
{
  upRankingSystem rs = getScenario1();
  PlayerMngr pm = rs->getPlayerMngr();
  MatchMngr mm = rs->getMatchMngr();
  upRankingDb db = getDirectDatabaseHandle();

  // get the four default players
  upPlayer p1 = pm.getPlayerById(1);
  ASSERT_TRUE(p1 != nullptr);
  upPlayer p2 = pm.getPlayerById(2);
  ASSERT_TRUE(p1 != nullptr);
  upPlayer p3 = pm.getPlayerById(3);
  ASSERT_TRUE(p1 != nullptr);
  upPlayer p4 = pm.getPlayerById(4);
  ASSERT_TRUE(p1 != nullptr);

  // prep fake timestamps and results
  LocalTimestamp now;
  LocalTimestamp fakeMatchTime(2000, 7, 1, 11, 0, 0);
  auto ms = MatchScore::fromString("21:18,21:16");
  ASSERT_TRUE(ms != nullptr);

  // test identical players
  ERR e;
  upMatch m = mm.stageNewMatch_Singles(*p1, *p1, *ms, fakeMatchTime, &e);
  ASSERT_EQ(nullptr, m);
  ASSERT_EQ(ERR::IDENTICAL_PLAYERS_IN_MATCH, e);

  // try one or two disabled players
  m = mm.stageNewMatch_Singles(*p3, *p4, *ms, fakeMatchTime, &e);
  ASSERT_EQ(nullptr, m);
  ASSERT_EQ(ERR::INACTIVE_PLAYERS_IN_MATCH, e);
  m = mm.stageNewMatch_Singles(*p3, *p1, *ms, fakeMatchTime, &e);
  ASSERT_EQ(nullptr, m);
  ASSERT_EQ(ERR::INACTIVE_PLAYERS_IN_MATCH, e);
  m = mm.stageNewMatch_Singles(*p2, *p4, *ms, fakeMatchTime, &e);
  ASSERT_EQ(nullptr, m);
  ASSERT_EQ(ERR::INACTIVE_PLAYERS_IN_MATCH, e);

  // make sure no match has been created to far
  DbTab* matchTab = db->getTab(TAB_MATCH);
  ASSERT_EQ(0, matchTab->length());

  // create a valid match
  m = mm.stageNewMatch_Singles(*p1, *p2, *ms, fakeMatchTime, &e);
  ASSERT_TRUE(m != nullptr);
  ASSERT_EQ(ERR::SUCCESS, e);
  ASSERT_EQ(1, matchTab->length());

  // check the entries
  TabRow r = matchTab->operator [](1);
  ASSERT_EQ(1, r.getInt(MA_WINNER1_REF));
  ASSERT_EQ(2, r.getInt(MA_LOSER1_REF));
  ASSERT_TRUE(r.getInt2(MA_WINNER2_REF)->isNull());
  ASSERT_TRUE(r.getInt2(MA_LOSER2_REF)->isNull());
  ASSERT_EQ("21:18,21:16", r[MA_RESULT]);
  ASSERT_EQ(fakeMatchTime.getRawTime(), r.getInt(MA_TIMESTAMP));
  ASSERT_EQ(MA_STATE_STAGED, r.getInt(MA_STATE));
  LocalTimestamp storeTime = r.getLocalTime(MA_MATCH_STORED_TIMESTAMP);
  ASSERT_TRUE(storeTime >= now);
  ASSERT_TRUE(storeTime.getRawTime() <= (now.getRawTime()+2));   // a grace period for the test case execution time


  // create a match with the need to swap results
  ms = MatchScore::fromString("21:18,16:21,17:21");
  ASSERT_TRUE(ms != nullptr);
  m = mm.stageNewMatch_Singles(*p1, *p2, *ms, fakeMatchTime, &e);
  ASSERT_TRUE(m != nullptr);
  ASSERT_EQ(ERR::SUCCESS, e);
  ASSERT_EQ(2, matchTab->length());

  // check the entries
  r = matchTab->operator [](2);
  ASSERT_EQ(2, r.getInt(MA_WINNER1_REF));
  ASSERT_EQ(1, r.getInt(MA_LOSER1_REF));
  ASSERT_TRUE(r.getInt2(MA_WINNER2_REF)->isNull());
  ASSERT_TRUE(r.getInt2(MA_LOSER2_REF)->isNull());
  ASSERT_EQ("18:21,21:16,21:17", r[MA_RESULT]);   // swapped!
  ASSERT_EQ(fakeMatchTime.getRawTime(), r.getInt(MA_TIMESTAMP));
  ASSERT_EQ(MA_STATE_STAGED, r.getInt(MA_STATE));
  storeTime = r.getLocalTime(MA_MATCH_STORED_TIMESTAMP);
  ASSERT_TRUE(storeTime >= now);
  ASSERT_TRUE(storeTime.getRawTime() <= (now.getRawTime()+2));   // a grace period for the test case execution time
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

