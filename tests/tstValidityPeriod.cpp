#include <string>
#include <gtest/gtest.h>

#include "RankingSystem.h"
#include "BasicTestClass.h"
#include "ValidityPeriod.h"
#include "RankingDataDefs.h"

using namespace std;
using namespace RankingApp;

TEST_F(BasicTestFixture, ValidityPeriod_Relation)
{
  upRankingSystem rs = getEmptyRankingSys();
  upRankingDb db = getDirectDatabaseHandle();
  PlayerMngr pm = rs->getPlayerMngr();

  // create a player, because we need a valid target
  // for the foreign key (player ref) in the validity tab
  auto p = pm.createNewPlayer("f", "l");
  ASSERT_TRUE(p != nullptr);

  // fake two period entries
  DbTab* vt = db->getTab(TAB_VALIDITY);
  LocalTimestamp t0{2000, 03, 01, 10, 00, 00};
  LocalTimestamp t1{2000, 03, 03, 10, 00, 00};
  LocalTimestamp t2{2000, 03, 05, 10, 00, 00};
  LocalTimestamp t3{2000, 03, 07, 10, 00, 00};
  ColumnValueClause cvc;
  cvc.addIntCol(VA_PLAYER_REF, 1);
  cvc.addDateTimeCol(VA_PERIOD_START, &t1);
  cvc.addDateTimeCol(VA_PERIOD_END, &t2);
  int newId = vt->insertRow(cvc);
  ASSERT_EQ(1, newId);
  cvc.clear();
  cvc.addIntCol(VA_PLAYER_REF, 1);
  cvc.addDateTimeCol(VA_PERIOD_START, &t2);
  newId = vt->insertRow(cvc);
  ASSERT_EQ(2, newId);

  // use the PlayerMngr to retrieve ValidityPeriod-objects
  // for both entries
  vector<ValidityPeriod> allPeriods = pm.getValidityPeriodsForPlayer(*p);
  ASSERT_EQ(2, allPeriods.size());

  // test the "closed" period
  ValidityPeriod prd = allPeriods.at(0);
  ASSERT_TRUE(prd.hasEndDate());
  ASSERT_EQ(ValidityPeriod::IS_BEFORE_PERIOD, prd.determineRelationToPeriod(t0));
  ASSERT_EQ(ValidityPeriod::IS_IN_PERIOD, prd.determineRelationToPeriod(t1));
  ASSERT_EQ(ValidityPeriod::IS_IN_PERIOD, prd.determineRelationToPeriod(t2));
  ASSERT_EQ(ValidityPeriod::IS_AFTER_PERIOD, prd.determineRelationToPeriod(t3));

  // test the "open" period
  prd = allPeriods.at(1);
  ASSERT_FALSE(prd.hasEndDate());
  ASSERT_EQ(ValidityPeriod::IS_BEFORE_PERIOD, prd.determineRelationToPeriod(t0));
  ASSERT_EQ(ValidityPeriod::IS_BEFORE_PERIOD, prd.determineRelationToPeriod(t1));
  ASSERT_EQ(ValidityPeriod::IS_IN_PERIOD, prd.determineRelationToPeriod(t2));
  ASSERT_EQ(ValidityPeriod::IS_IN_PERIOD, prd.determineRelationToPeriod(t3));
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

