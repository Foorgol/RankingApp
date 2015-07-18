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
    static unique_ptr<RankingSystem> get(const string& fname, ERR* err);
    static unique_ptr<RankingSystem> createEmpty(const string& fname, ERR* err);
    static bool isValidFilename(const string& fname);

    PlayerMngr getPlayerMngr();
    MatchMngr getMatchMngr();

    void recalcRankings(int maxYear=-1, int maxMonth=-1, int maxDay=-1);
    void recalcRankings(const LocalTimestamp& maxTimeIncluded);
    int RankingClassToInt(RANKING_CLASS rc) const;

    ERR confirmMatchAndUpdateRanking(const Match& ma);
    PlainRankingEntryList getSortedRanking(RANKING_CLASS rankClass) const;

    int getInitialScoreForNewPlayer(RANKING_CLASS rankClass, int startYear, int startMonth, int startDay);

    void setLogLevel(int newLvl);

  protected:
    upRankingDb db;
    RankingSystem(upRankingDb _db);
    static unique_ptr<RankingSystem> doInit(const string& fname, bool doCreateNew, ERR* err);
    PlainRankingEntryList recalcRanking(RANKING_CLASS rankClass, const LocalTimestamp& maxTimeIncluded);
    void sortPlainRankingEntryListInPlace(PlainRankingEntryList& rel, const RANKING_CLASS& rankClass);
    void assignRanksAndValuesToSortedPlainRankingEntryListInPlace(PlainRankingEntryList& rel) const;
  };

  typedef unique_ptr<RankingSystem> upRankingSystem;
}

#endif  /* RANKINGSYS_H */
