
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
}

//----------------------------------------------------------------------------

void RankingApp::RankingDb::populateViews()
{

}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


