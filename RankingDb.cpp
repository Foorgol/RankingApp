
#include "TableCreator.h"

#include "RankingDb.h"
#include "RankingDataDefs.h"
#include "HelperFunc.h"

void RankingApp::RankingDb::populateTables()
{
  TableCreator tc{this};

  // the player table
  tc.addVarchar(PL_FIRSTNAME, MAX_NAME_LEN, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addVarchar(PL_LASTNAME, MAX_NAME_LEN, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.createTableAndResetCreator(TAB_PLAYER);

  // the validity table
  tc.addForeignKey(VA_PLAYER_REF, TAB_PLAYER, CONSISTENCY_ACTION::RESTRICT);
  tc.addInt(VA_PERIOD_START, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addInt(VA_PERIOD_END, false, CONFLICT_CLAUSE::__NOT_SET, false, CONFLICT_CLAUSE::__NOT_SET);
  tc.createTableAndResetCreator(TAB_VALIDITY);

  // the table of matches
  tc.addForeignKey(MA_WINNER1_REF, TAB_PLAYER, CONSISTENCY_ACTION::RESTRICT);
  tc.addForeignKey(MA_WINNER2_REF, TAB_PLAYER, CONSISTENCY_ACTION::RESTRICT);
  tc.addForeignKey(MA_LOSER1_REF, TAB_PLAYER, CONSISTENCY_ACTION::RESTRICT);
  tc.addForeignKey(MA_LOSER2_REF, TAB_PLAYER, CONSISTENCY_ACTION::RESTRICT);
  tc.addVarchar(MA_RESULT, MAX_RESULT_LEN, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addInt(MA_DATE, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addInt(MA_STATE, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addInt(MA_MATCH_STORED_TIMESTAMP, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addInt(MA_MATCH_CONFIRMED_TIMESTAMP, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.createTableAndResetCreator(TAB_MATCH);

  // the rankings managed by this application
  tc.addVarchar(RC_NAME, MAX_NAME_LEN, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addInt(RC_IS_SINGLES, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.createTableAndResetCreator(TAB_RANKING_CLASSES);

  // the score table
  tc.addForeignKey(SC_PLAYER_REF, TAB_PLAYER, CONSISTENCY_ACTION::RESTRICT);
  tc.addInt(SC_SCORE, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addInt(SC_DATE, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addInt(SC_SEQ_NUM, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addInt(SC_TYPE, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addForeignKey(SC_MATCH_REF, TAB_MATCH, CONSISTENCY_ACTION::RESTRICT);
  tc.addForeignKey(SC_RANK_CLASS_REF, TAB_RANKING_CLASSES, CONSISTENCY_ACTION::RESTRICT);
  tc.createTableAndResetCreator(TAB_SCORE);

  // the ranking
  // use a dedicated function for this, because this table
  // is frequently dropped and re-created during operation
  dropAndCreateRankingTab();

}

//----------------------------------------------------------------------------

void RankingApp::RankingDb::populateViews()
{

}

//----------------------------------------------------------------------------

void RankingApp::RankingDb::dropAndCreateRankingTab()
{
  // drop the table if it exists
  string sql = "DROP TABLE IF EXISTS " + string(TAB_RANKING);
  execNonQuery(sql);

  // create an empty table
  TableCreator tc{this};
  tc.addForeignKey(RA_PLAYER_REF, TAB_PLAYER, CONSISTENCY_ACTION::RESTRICT);
  tc.addForeignKey(RA_RANK_CLASS_REF, TAB_RANKING_CLASSES, CONSISTENCY_ACTION::RESTRICT);
  tc.addInt(RA_RANK, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addInt(RA_VALUE, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.addVarchar(RA_SCORE_QUEUE, MAX_SCORE_QUEUE_LEN, false, CONFLICT_CLAUSE::__NOT_SET, true, CONFLICT_CLAUSE::ROLLBACK);
  tc.createTableAndResetCreator(TAB_RANKING);

}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


