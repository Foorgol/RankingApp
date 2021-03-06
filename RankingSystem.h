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

  class RankingClass;

  class RankingSystem
  {
  public:
    static unique_ptr<RankingSystem> get(const string& fname, ERR* err);
    static unique_ptr<RankingSystem> createEmpty(const string& fname, ERR* err);
    static bool isValidFilename(const string& fname);

    PlayerMngr getPlayerMngr();
    MatchMngr getMatchMngr();

    void recalcRankings(int maxSeqNumIncluded=-1);
    PlainRankingEntryList recalcRanking(const RankingClass& rankClass, int maxSeqNumIncluded);
    void storeRankingEntries(const PlainRankingEntryList& singlesRanking_sorted, const PlainRankingEntryList& doublesRanking_sorted);
    int RankingClassToInt(RankingClass rc) const;

    ERR confirmMatchAndUpdateRanking(const Match& ma);
    PlainRankingEntryList getSortedRanking(const RankingClass& rankClass) const;

    int getInitialScoreForNewPlayer(const RankingClass& rankClass, date startDate);

    void setLogLevel(SeverityLevel newLvl);

  protected:
    upRankingDb db;
    RankingSystem(upRankingDb _db);
    static unique_ptr<RankingSystem> doInit(const string& fname, bool doCreateNew, ERR* err);
    void sortPlainRankingEntryListInPlace(PlainRankingEntryList& rel, const RankingClass& rankClass);
    void assignRanksAndValuesToSortedPlainRankingEntryListInPlace(PlainRankingEntryList& rel) const;
    void rewriteMatchScores(int maxSeqNumIncluded);
    date getDateForScoreSeqNum(int seqNum);
  };

  typedef unique_ptr<RankingSystem> upRankingSystem;
}

#endif  /* RANKINGSYS_H */
