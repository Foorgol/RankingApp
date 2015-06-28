#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <queue>
#include <string>
#include "ConvenienceFuncs.h"

using namespace std;

namespace RankingApp {

  class ScoreQueue
  {
  public:
    ScoreQueue(int _queueSize);
    ScoreQueue(int _queueSize, int initialScore);
    virtual ~ScoreQueue(){}

    static unique_ptr<ScoreQueue> fromString(int _queueSize, const string& scoreString);
    void pushScore(int score);

    string toString() const;

  private:
    queue<int> scoreQueue;
    size_t queueSize;
  };

  typedef unique_ptr<ScoreQueue> upScoreQueue;

}

#endif  /* PLAYER_H */