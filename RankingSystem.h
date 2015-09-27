#ifndef RANKINGSYS_H
#define RANKINGSYS_H

#include <memory>
#include <string>

#include "RankingDb.h"
#include "RankingDataDefs.h"
#include "RankingErrCodes.h"
#include "PlayerMngr.h"
#include "MatchMngr.h"
#include "ScoreQueue.h"

using namespace SqliteOverlay;

namespace RankingApp {

  // a supporting data structure used during ranking calculation (e.g., for sorting)
  //
  // has no database connection
  class PlainRankingEntry
  {
  public:
    int playerRef;
    int rank;
    int value;
    ScoreQueue scores;
  };
  typedef vector<PlainRankingEntry> PlainRankingEntryList;

  class RankingSystem
  {
  public:
    static constexpr int SEQ_NUM__NO_MATCHES = -1;
    static constexpr int SEQ_NUM__ALL_MATCHES__NO_PENALTY = 99999;
    static constexpr int SEQ_NUM__ALL_MATCHES__ALL_PENALTY = 99998;
    static unique_ptr<RankingSystem> get(const string& fname, ERR* err);
    static unique_ptr<RankingSystem> createEmpty(const string& fname, ERR* err);
    static bool isValidFilename(const string& fname);

    PlayerMngr getPlayerMngr();
    MatchMngr getMatchMngr();

    void recalcRankings(int maxSeqNumIncluded);
    PlainRankingEntryList recalcRanking(RANKING_CLASS rankClass, int maxSeqNumIncluded);
    void storeRankingEntries(const PlainRankingEntryList& singlesRanking_sorted, const PlainRankingEntryList& doublesRanking_sorted);
    int RankingClassToInt(RANKING_CLASS rc) const;

    ERR confirmMatchAndUpdateRanking(const Match& ma);
    PlainRankingEntryList getSortedRanking(RANKING_CLASS rankClass) const;

    int getInitialScoreForNewPlayer(RANKING_CLASS rankClass, int startYear, int startMonth, int startDay);

    void setLogLevel(int newLvl);

    LocalTimestamp getLatestScoreEventTimestamp() const;

  protected:
    upRankingDb db;
    RankingSystem(upRankingDb _db);
    static unique_ptr<RankingSystem> doInit(const string& fname, bool doCreateNew, ERR* err);
    void sortPlainRankingEntryListInPlace(PlainRankingEntryList& rel, const RANKING_CLASS& rankClass);
    void assignRanksAndValuesToSortedPlainRankingEntryListInPlace(PlainRankingEntryList& rel) const;
    void rewriteMatchScores(int maxSeqNumIncluded);
    string getIsoDateForScoreSeqNum(int seqNum);
  };

  typedef unique_ptr<RankingSystem> upRankingSystem;
}

#endif  /* RANKINGSYS_H */
