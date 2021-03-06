
#include <Sloppy/DateTime/DateAndTime.h>

#include "MatchMngr.h"
#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"
#include "PlayerMngr.h"
#include "RankingSystem.h"

using namespace RankingApp;


MatchMngr::MatchMngr(RankingDb* _db, RankingSystem* _rs)
  :GenericObjectManager(_db, TAB_MATCH), rs(_rs)
{
  if (_db == nullptr)
  {
    throw std::invalid_argument("Received nullptr as database handle");
  }
  if (_rs == nullptr)
  {
    throw std::invalid_argument("Received nullptr as ranking system handle");
  }
}

//----------------------------------------------------------------------------

upMatch MatchMngr::stageNewMatch_Singles(const Player& player1, const Player& player2, MatchScore& score, const boost::gregorian::date& date, ERR* err) const
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
  PlayerMngr pm{db, rs};
  if (!(pm.isPlayerEnabledOnSpecificDate(player1, date)))
  {
    ConvenienceFuncs::setErr(err, ERR::INACTIVE_PLAYERS_IN_MATCH);
    return nullptr;
  }
  if (!(pm.isPlayerEnabledOnSpecificDate(player2, date)))
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
  cvc.addDateCol(MA_DATE, date);
  cvc.addIntCol(MA_STATE, static_cast<int>(MatchState::Staged));
  UTCTimestamp now{};
  cvc.addDateTimeCol(MA_MATCH_STORED_TIMESTAMP, &now);
  int newId = tab->insertRow(cvc);
  if (newId < 1)
  {
    ConvenienceFuncs::setErr(err, ERR::DATABASE_ERROR);
    return nullptr;
  }

  ConvenienceFuncs::setErr(err, ERR::SUCCESS);
  return getMatchById(newId);
}

//----------------------------------------------------------------------------

ERR MatchMngr::confirmMatch(const Match& ma) const
{
  // make sure the match is "staged"
  if (ma.getState() != MatchState::Staged)
  {
    return ERR::MATCH_NOT_STAGED;
  }

  // update the match entry
  int newState = static_cast<int>(MatchState::Confirmed);
  ColumnValueClause cvc;
  cvc.addIntCol(MA_STATE, newState);
  UTCTimestamp now{};
  cvc.addDateTimeCol(MA_MATCH_CONFIRMED_TIMESTAMP, &now);
  ma.row.update(cvc);

  // IMPORTANT: an (incremental) update of the ranking
  // has to be triggered elsewhere!
  return ERR::SUCCESS;
}

//----------------------------------------------------------------------------

upMatch MatchMngr::getMatchById(int id) const
{
  return getSingleObjectByColumnValue<Match>("id", id);
}

//----------------------------------------------------------------------------

upMatch MatchMngr::getLatestMatchForPlayer(const Player& p, const RankingClass& rankClass, bool confirmedMatchesOnly) const
{
  //
  // THIS FUNCTION NEEDS TO BE RE-IMPLEMENTED
  //

  return nullptr;

  /*
  string sql = ("SELECT id FROM ") + string(TAB_MATCH) + " WHERE (";
  if (rankClass == RANKING_CLASS::DOUBLES)
  {
    sql += string(MA_WINNER1_REF) + "= @player_id OR ";
    sql += string(MA_WINNER2_REF) + "= @player_id OR ";
    sql += string(MA_LOSER1_REF) + "= @player_id OR ";
    sql += string(MA_LOSER2_REF) + "= @player_id ) ";
  }
  if (rankClass == RANKING_CLASS::SINGLES)
  {
    sql += string(MA_WINNER1_REF) + "= @player_id OR ";
    sql += string(MA_LOSER1_REF) + "= @player_id ) AND ";
    sql += string(MA_WINNER2_REF) + " IS NULL AND ";
    sql += string(MA_LOSER2_REF) + " IS NULL ";
  }
  if (confirmedMatchesOnly)
  {
    sql += "AND " + string(MA_STATE) + "= @stat ";
  }
  sql += "ORDER BY " + string(MA_ISODATE) + " DESC";
  sql += ", " + string(ma);
  
  WhereClause w;
  w.addIntCol();
      

  upSqlStatement stmt = db->prepStatement(sql);
  stmt->bindInt(1, p.getId());
  if (confirmedMatchesOnly)
  {
    stmt->bindInt(2, MA_STATE_CONFIRMED);
  }

  // execute the statement and check for a result
  db->execContentQuery(stmt);
  if (stmt->hasData())
  {
    int matchId;
    stmt->getInt(0, &matchId);
    return getMatchById(matchId);
  }

  // no match found
  return nullptr;
  */
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

