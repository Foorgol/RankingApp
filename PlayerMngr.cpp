
#include "PlayerMngr.h"
#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"

using namespace RankingApp;


PlayerMngr::PlayerMngr(RankingDb* _db)
  :GenericObjectManager(_db), db(_db)
{
  if (_db == nullptr)
  {
    throw std::invalid_argument("Received nullptr as database handle");
  }

  playerTab = db->getTab(TAB_PLAYER);
}

//----------------------------------------------------------------------------

upPlayer PlayerMngr::createNewPlayer(const string& firstName, const string& lastName, ERR* err) const
{
  string fn = firstName;
  ConvenienceFuncs::trim(fn);
  string ln = lastName;
  ConvenienceFuncs::trim(ln);

  // make sure the player names are not empty
  if ((fn.empty()) || (ln.empty()))
  {
    ConvenienceFuncs::setErr(err, ERR::INVALID_NAME);
    return nullptr;
  }

  // check if a player of that name already exists
  auto pl = getPlayerByName(fn, ln);
  if (pl != nullptr)
  {
    ConvenienceFuncs::setErr(err, ERR::PLAYER_EXISTS);
    return nullptr;
  }

  // create the player
  ColumnValueClause cvc;
  cvc.addStringCol(PL_FIRSTNAME, fn);
  cvc.addStringCol(PL_LASTNAME, ln);
  int newId = playerTab->insertRow(cvc);
  if (newId < 1)
  {
    ConvenienceFuncs::setErr(err, ERR::DATABASE_ERROR);
    return nullptr;
  }

  ConvenienceFuncs::setErr(err, ERR::SUCCESS);
  return getPlayerById(newId);
}

//----------------------------------------------------------------------------

upPlayer PlayerMngr::getPlayerByName(const string& firstName, const string& lastName) const
{
  WhereClause w;
  w.addStringCol(PL_FIRSTNAME, firstName);
  w.addStringCol(PL_LASTNAME, lastName);
  return getSingleObjectByWhereClause<Player>(*playerTab, w);
}

//----------------------------------------------------------------------------

upPlayer PlayerMngr::getPlayerById(int id) const
{
  return getSingleObjectByColumnValue<Player>(*playerTab, "id", id);
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

