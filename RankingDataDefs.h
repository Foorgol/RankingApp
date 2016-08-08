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

#define MA_STATE_STAGED 1
#define MA_STATE_CONFIRMED 2

// score events
#define TAB_SCORE "ScoreEvent"
#define SC_PLAYER_REF "PlayerRef"
#define SC_SCORE "Score"
#define SC_DATE "Date"
#define SC_SEQ_NUM "SequenceNumber"
#define SC_TYPE "ScoringType"
#define SC_MATCH_REF "MatchRef"
#define SC_SCORE_TARGET "ScoreTarget"   // Singles or doubles

// score event types
// the types are numbered in order of precendence:
// in case of identical timestamps for a score event,
// initial scores go first, then matches, then penalty scores
#define SC_TYPE_INITIAL 1
#define SC_TYPE_MATCH 2
#define SC_TYPE_IUM 3
#define SC_TYPE_OTHER 4
#define SC_TYPE_LAZYNESS 5

#define SC_SCORE_TARGET_SINGLES 1
#define SC_SCORE_TARGET_DOUBLES 2

// the ranking itself
#define TAB_RANKING "Ranking"
#define RA_PLAYER_REF "PlayerRef"
#define RA_RANKING_CLASS "RankingClass"   // singles or doubles
#define RA_RANK "Rank"
#define RA_VALUE "Value"
#define RA_SCORE_QUEUE "ScoreQueue"

#define RA_RANKING_CLASS_SINGLES SC_SCORE_TARGET_SINGLES
#define RA_RANKING_CLASS_DOUBLES SC_SCORE_TARGET_DOUBLES





enum class RANKING_CLASS
{
  SINGLES,
  DOUBLES
};

enum class MATCH_STATE
{
  STAGED,
  CONFIRMED,
};


#define SCORE_QUEUE_DEPTH 5
#define MAX_RANK_VALUE 120
#define RANK_VALUE_STEP 2
#define MAX_ACTIVE_PLAYER_COUNT (MAX_RANK_VALUE / RANK_VALUE_STEP)

#endif  /* RANKINGDATADEFS_H */
