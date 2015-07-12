#ifndef SCOREQUEUE_H
#define SCOREQUEUE_H

#include <memory>
#include <queue>
#include <string>
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"

using namespace std;

namespace RankingApp {

  class ScoreQueue
  {
  public:
    ScoreQueue(int _queueSize=SCORE_QUEUE_DEPTH);
    ScoreQueue(int _queueSize, int initialScore);
    virtual ~ScoreQueue(){}

    static unique_ptr<ScoreQueue> fromString(int _queueSize, const string& scoreString);
    void pushScore(int score);

    string toString() const;
    int getSum() const;

  private:
    queue<int> scoreQueue;
    size_t queueSize;
    int curSum;
  };

  typedef unique_ptr<ScoreQueue> upScoreQueue;

}

#endif  /* SCOREQUEUE_H */
