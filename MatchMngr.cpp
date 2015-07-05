
#include "MatchMngr.h"
#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"
#include "PlayerMngr.h"

using namespace RankingApp;


MatchMngr::MatchMngr(RankingDb* _db)
  :GenericObjectManager(_db), db(_db)
{
  if (_db == nullptr)
  {
    throw std::invalid_argument("Received nullptr as database handle");
  }

  matchTab = db->getTab(TAB_MATCH);
}

//----------------------------------------------------------------------------

upMatch MatchMngr::stageNewMatch_Singles(const Player& player1, const Player& player2, MatchScore& score, const LocalTimestamp& timestamp, ERR* err) const
{
  // players may not be identical
  if (player1 == player2)
  {
    ConvenienceFuncs::setErr(err, ERR::IDENTICAL_PLAYERS_IN_MATCH);
    return nullptr;
  }

  // make sure the score is valid
  if (!(score.isValidScore()))
  {
    ConvenienceFuncs::setErr(err, ERR::INVALID_SCORE);
    return nullptr;
  }

  // all players must have been enabled when the match took place
  PlayerMngr pm{db};
  string isoDate = timestamp.getISODate();
  if (!(pm.isPlayerEnabledOnSpecificDate(player1, isoDate)))
  {
    ConvenienceFuncs::setErr(err, ERR::INACTIVE_PLAYERS_IN_MATCH);
    return nullptr;
  }
  if (!(pm.isPlayerEnabledOnSpecificDate(player2, isoDate)))
  {
    ConvenienceFuncs::setErr(err, ERR::INACTIVE_PLAYERS_IN_MATCH);
    return nullptr;
  }

  // everthing is okay, let's file the match
  ColumnValueClause cvc;
  if (score.getWinner() == 1)
  {
    cvc.addIntCol(MA_WINNER1_REF, player1.getId());
    cvc.addIntCol(MA_LOSER1_REF, player2.getId());
  } else {
    cvc.addIntCol(MA_WINNER1_REF, player2.getId());
    cvc.addIntCol(MA_LOSER1_REF, player1.getId());
    score.swapPlayers();  // winner score is always the first in the game score
  }
  cvc.addStringCol(MA_RESULT, score.toString());
  cvc.addDateTimeCol(MA_TIMESTAMP, &timestamp);
  cvc.addIntCol(MA_STATE, MA_STATE_STAGED);
  LocalTimestamp now;
  cvc.addDateTimeCol(MA_MATCH_STORED_TIMESTAMP, &now);
  int newId = matchTab->insertRow(cvc);
  if (newId < 1)
  {
    ConvenienceFuncs::setErr(err, ERR::DATABASE_ERROR);
    return nullptr;
  }

  ConvenienceFuncs::setErr(err, ERR::SUCCESS);
  return getMatchById(newId);
}

//----------------------------------------------------------------------------

upMatch MatchMngr::getMatchById(int id) const
{
  return getSingleObjectByColumnValue<Match>(*matchTab, "id", id);
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

