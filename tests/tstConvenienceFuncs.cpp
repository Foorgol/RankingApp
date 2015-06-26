#include <string>
#include <gtest/gtest.h>

#include "ConvenienceFuncs.h"

using namespace std;
using namespace RankingApp;

TEST(ConvenienceFuncs, Trim)
{
  string s = " \nabc  \n\t";
  string sCopy = s;

  // test left trim
  ConvenienceFuncs::ltrim(s);
  ASSERT_EQ("abc  \n\t", s);

  // test right trim
  s = sCopy;
  ConvenienceFuncs::rtrim(s);
  ASSERT_EQ(" \nabc", s);

  // test both trims
  s = sCopy;
  ConvenienceFuncs::trim(s);
  ASSERT_EQ("abc", s);
}
