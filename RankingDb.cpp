

#include "RankingDb.h"
#include "RankingDataDefs.h"
#include "HelperFunc.h"

void RankingApp::RankingDb::populateTables()
{
  StringList col;
  auto addStandardCol = [&col](const char* colName, const string& colType) {
    col.push_back(string(colName) + " " + colType);
  };
  auto addIntCol = [&addStandardCol](const char* colName) {
    addStandardCol(colName, "INTEGER");
  };

  string defaultNameColDef = " VARCHAR(" + to_string(MAX_NAME_LEN) + ")";

  // the player table
  col.push_back(PL_FIRSTNAME + defaultNameColDef);
  col.push_back(PL_LASTNAME + defaultNameColDef);
  tableCreationHelper(TAB_PLAYER, col);
  col.clear();

  // the validity table
  col.push_back(genForeignKeyClause(VA_PLAYER_REF, TAB_PLAYER));
  addIntCol(VA_PERIOD_START);
  addIntCol(VA_PERIOD_END);
  tableCreationHelper(TAB_VALIDITY, col);
  col.clear();

  // the table of matches
  col.push_back(genForeignKeyClause(MA_WINNER1_REF, TAB_PLAYER));
  col.push_back(genForeignKeyClause(MA_WINNER2_REF, TAB_PLAYER));
  col.push_back(genForeignKeyClause(MA_LOSER1_REF, TAB_PLAYER));
  col.push_back(genForeignKeyClause(MA_LOSER2_REF, TAB_PLAYER));
  addStandardCol(MA_RESULT, "VARCHAR(40)");
  addIntCol(MA_DATE);
  addIntCol(MA_STATE);
  addIntCol(MA_MATCH_STORED_TIMESTAMP);
  addIntCol(MA_MATCH_CONFIRMED_TIMESTAMP);
  tableCreationHelper(TAB_MATCH, col);
  col.clear();

  // the score table
  col.push_back(genForeignKeyClause(SC_PLAYER_REF, TAB_PLAYER));
  addIntCol(SC_SCORE);
  addIntCol(SC_DATE);
  addIntCol(SC_SEQ_NUM);
  addIntCol(SC_TYPE);
  col.push_back(genForeignKeyClause(SC_MATCH_REF, TAB_MATCH));
  addIntCol(SC_SCORE_TARGET);
  tableCreationHelper(TAB_SCORE, col);
  col.clear();

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
  StringList col;
  auto addStandardCol = [&col](const char* colName, const string& colType) {
    col.push_back(string(colName) + " " + colType);
  };
  auto addIntCol = [&addStandardCol](const char* colName) {
    addStandardCol(colName, "INTEGER");
  };

  col.push_back(genForeignKeyClause(RA_PLAYER_REF, TAB_PLAYER));
  addIntCol(RA_RANKING_CLASS);
  addIntCol(RA_RANK);
  addIntCol(RA_VALUE);
  addStandardCol(RA_SCORE_QUEUE, "VARCHAR(40)");
  tableCreationHelper(TAB_RANKING, col);
  col.clear();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


