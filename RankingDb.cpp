

#include "RankingDb.h"
#include "RankingDataDefs.h"
#include "HelperFunc.h"

void RankingApp::RankingDb::populateTables()
{
  StringList col;

  string defaultNameColDef = " VARCHAR(" + to_string(MAX_NAME_LEN) + ")";

  // the player table
  col.push_back(PL_FIRSTNAME + defaultNameColDef);
  col.push_back(PL_LASTNAME + defaultNameColDef);
  tableCreationHelper(TAB_PLAYER, col);
  col.clear();

  // the validity table
  col.push_back(genForeignKeyClause(VA_PLAYER_REF, TAB_PLAYER));
  col.push_back(string(VA_PERIOD_START) + " INTEGER");
  col.push_back(string(VA_PERIOD_END) + " INTEGER");
  tableCreationHelper(TAB_VALIDITY, col);
  col.clear();
}

//----------------------------------------------------------------------------

void RankingApp::RankingDb::populateViews()
{

}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


