
#include "Player.h"
#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"
#include "TabRow.h"

using namespace RankingApp;

string Player::getName__FirstNameFirst() const
{
  return row[PL_FIRSTNAME] + " " + row[PL_LASTNAME];
}

//----------------------------------------------------------------------------

string Player::getName__LastNameFirst() const
{
  return row[PL_LASTNAME] + ", " + row[PL_FIRSTNAME];
}

//----------------------------------------------------------------------------

string Player::getLastName() const
{
  return row[PL_LASTNAME];
}

//----------------------------------------------------------------------------

string Player::getFirstName() const
{
  return row[PL_FIRSTNAME];
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

Player::Player(SqliteDatabase* db, int rowId)
:GenericDatabaseObject(db, TAB_PLAYER, rowId)
{
}

//----------------------------------------------------------------------------

Player::Player(SqliteDatabase* db, TabRow row)
:GenericDatabaseObject(db, row)
{
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
