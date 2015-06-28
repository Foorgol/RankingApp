#include <string>
#include <gtest/gtest.h>

#include "ScoreQueue.h"

using namespace std;
using namespace RankingApp;

TEST(ScoreQueue, ctor)
{
  ScoreQueue q{5, 42};
  ASSERT_EQ("42, 42, 42, 42, 42", q.toString());

  q = ScoreQueue(4);
  ASSERT_EQ("0, 0, 0, 0", q.toString());
}

//----------------------------------------------------------------------------

TEST(ScoreQueue, fromString)
{
  string s = "1,2,3,4";
  auto q = ScoreQueue::fromString(4, s);
  ASSERT_EQ("1, 2, 3, 4", q->toString());

  // invalid number of elements
  q = ScoreQueue::fromString(3, s);
  ASSERT_EQ(q, nullptr);

  // invalid elements
  q = ScoreQueue::fromString(3, "1,2,x,4");
  ASSERT_EQ(q, nullptr);

  // empty elements are okay, though
  q = ScoreQueue::fromString(4, "1,2,,,,3,4");
  ASSERT_EQ("1, 2, 3, 4", q->toString());
}

//----------------------------------------------------------------------------

TEST(ScoreQueue, push)
{
  auto q = ScoreQueue::fromString(4, "1,2,3,4");
  ASSERT_EQ("1, 2, 3, 4", q->toString());
  q->pushScore(5);
  ASSERT_EQ("2, 3, 4, 5", q->toString());
}

//----------------------------------------------------------------------------
