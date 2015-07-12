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
#define MA_TIMESTAMP "Timestamp"
#define MA_STATE "State"  // "Staged" or "Confirmed"
#define MA_MATCH_STORED_TIMESTAMP "MatchStoredAt"

#define MA_STATE_STAGED 1
#define MA_STATE_CONFIRMED 2

// score events
#define TAB_SCORE "ScoreEvent"
#define SC_PLAYER_REF "PlayerRef"
#define SC_SCORE "Score"
#define SC_TIMESTAMP "Timestamp"
#define SC_TYPE "ScoringType"
#define SC_MATCH_REF "MatchRef"
#define SC_SCORE_TARGET "ScoreTarget"   // Singles or doubles

#define SC_TYPE_MATCH 1
#define SC_TYPE_LAZYNESS 2
#define SC_TYPE_INITIAL 3
#define SC_TYPE_IUM 4
#define SC_TYPE_OTHER 5

#define SC_SCORE_TARGET_SINGLES 1
#define SC_SCORE_TARGET_DOUBLES 2

// the ranking itself
#define TAB_RANKING "Ranking"
#define RA_PLAYER_REF "PlayerRef"
#define RA_RANKING_CLASS "RankingClass"   // singles or doubles
#define RA_RANK "Rank"
#define RA_VALUE "Value"
#define RA_SCORE_QUEUE "ScoreQueue"

#define RA_RANKING_CLASS_SINGLES 1
#define RA_RANKING_CLASS_DOUBLES 2


#endif  /* RANKINGDATADEFS_H */
