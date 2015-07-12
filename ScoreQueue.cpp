
#include "ScoreQueue.h"
#include "ConvenienceFuncs.h"

using namespace RankingApp;

ScoreQueue::ScoreQueue(int _queueSize)
  :ScoreQueue(_queueSize, 0)
{
}

//----------------------------------------------------------------------------

ScoreQueue::ScoreQueue(int _queueSize, int initialScore)
  :queueSize(_queueSize)
{
  if (queueSize < 1)
  {
    throw std::invalid_argument("Queuesize must be greater or equal to one!");
  }
  if (initialScore < 0)
  {
    throw std::invalid_argument("Can't store scores less than zero!");
  }

  for (int i=0; i < queueSize; ++i)
  {
    scoreQueue.push(initialScore);
  }

  curSum = queueSize * initialScore;
}

//----------------------------------------------------------------------------

upScoreQueue ScoreQueue::fromString(int _queueSize, const string& scoreString)
{
  // the string shall be a comma-separated list of integers
  // with the oldest score being on the left side of the string
  StringList scoreItems = ConvenienceFuncs::splitString(scoreString, ',');

  // generate the result object
  upScoreQueue result = upScoreQueue(new ScoreQueue(_queueSize, 0));

  // iterate through the score items one by one from left
  // to right and push them into the queue
  int elemCount = 0;
  for (string _score : scoreItems)
  {
    // skip empty strings
    ConvenienceFuncs::trim(_score);
    if (_score.empty())
    {
      continue;
    }

    // try to convert the string into an int
    int score;
    try
    {
      score = stoi(_score);
    } catch (exception e)
    {
      return nullptr;   // error, one item could not be converted to int
    }

    // return with error in case of negative values
    if (score < 0)
    {
      return nullptr;
    }

    // store the element
    result->pushScore(score);
    ++elemCount;
  }

  // make sure we have enough items in the queue
  if (elemCount != _queueSize)
  {
    return nullptr;
  }

  return result;
}

//----------------------------------------------------------------------------

void ScoreQueue::pushScore(int score)
{
  if (score < 0)
  {
    throw std::invalid_argument("Can't store scores less than zero!");
  }

  scoreQueue.push(score);

  // all constructors initialize the queue with the necessary
  // number of elements.
  //
  // so whenever we pushed something using this method,
  // we need to pop the oldest entry immediately afterwards
  int oldestScore = scoreQueue.front();
  scoreQueue.pop();

  // update the internal score sum
  curSum = curSum - oldestScore + score;
}

//----------------------------------------------------------------------------

string ScoreQueue::toString() const
{
  // create a copy of the queue
  auto clone = *this;

  // conert to CSV, oldest first
  string result;
  while (!(clone.scoreQueue.empty()))
  {
    if (result.size() > 0)
    {
      result += ", ";
    }
    result += to_string(clone.scoreQueue.front());
    clone.scoreQueue.pop();
  }

  return result;
}

//----------------------------------------------------------------------------

int ScoreQueue::getSum() const
{
  return curSum;
}


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


