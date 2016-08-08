#ifndef RANKINGDATADEFS_H
#define RANKINGDATADEFS_H


// a table of all managed players
#define TAB_PLAYER "Player"
#define PL_FIRSTNAME "FirstName"
#define PL_LASTNAME "LastName"

// a table with activity / validity perdiods for players
#define TAB_VALIDITY "ValidityPeriod"
#define VA_PLAYER_REF "PlayerRef"
#define VA_PERIOD_START "PeriodStart"
#define VA_PERIOD_END "PeriodEnd"

// matches
#define TAB_MATCH "Match"
#define MA_WINNER1_REF "Winner1Ref"
#define MA_WINNER2_REF "Winner2Ref"
#define MA_LOSER1_REF "Loser1Ref"
#define MA_LOSER2_REF "Loser2Ref"
#define MA_RESULT "Result"
#define MA_DATE "Date"
#define MA_STATE "State"  // "Staged" or "Confirmed"
#define MA_MATCH_STORED_TIMESTAMP "MatchStoredAt"
#define MA_MATCH_CONFIRMED_TIMESTAMP "MatchConfirmedAt"

// a list of managed rankings (singles, doubles, mixed, U19, ...)
#define TAB_RANKING_CLASSES "RankingClasses"
#define RC_NAME "Name"
#define RC_IS_SINGLES "IsSingles"

// score events
#define TAB_SCORE "ScoreEvent"
#define SC_PLAYER_REF "PlayerRef"
#define SC_SCORE "Score"
#define SC_DATE "Date"
#define SC_SEQ_NUM "SequenceNumber"
#define SC_TYPE "ScoringType"
#define SC_MATCH_REF "MatchRef"
#define SC_RANK_CLASS_REF "RankingClassRef"

// the ranking itself
#define TAB_RANKING "Ranking"
#define RA_PLAYER_REF "PlayerRef"
#define RA_RANK_CLASS_REF "RankingClassRef"
#define RA_RANK "Rank"
#define RA_VALUE "Value"
#define RA_SCORE_QUEUE "ScoreQueue"


namespace RankingApp
{

  enum class MatchState
  {
    Staged,
    Confirmed,
  };

  // score event types
  // the types are listed in order of precendence:
  // in case of identical timestamps for a score event,
  // initial scores go first, then matches, then penalty scores
  //
  // when converted to int and stored in the database, "Initial"
  // gets "1", "Match" gets "2", etc.
  //
  // ==> sorting in the SQL-call or by int-representation yields
  // the correct order!
  enum class ScoreType
  {
    Initial,
    Match,
    IUM,
    Other,
    Lazyness
  };

  static constexpr int ScoreQueueDepth = 5;
  static constexpr int MaxRankValue = 120;
  static constexpr int RankValueStep = 2;
  static constexpr int MaxActivePlayerCount = (MaxRankValue / RankValueStep);
}

#endif  /* RANKINGDATADEFS_H */
