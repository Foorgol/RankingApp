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

//----------------------------------------------------------------------------

TEST(ConvenienceFuncs, Split)
{
  string s = "1, 2,,4  ,  abc";

  StringList items = ConvenienceFuncs::splitString(s, ',');
  ASSERT_EQ(5, items.size());
  ASSERT_EQ("1", items.at(0));
  ASSERT_EQ(" 2", items.at(1));
  ASSERT_EQ("", items.at(2));
  ASSERT_EQ("4  ", items.at(3));
  ASSERT_EQ("  abc", items.at(4));

  s = "";
  items = ConvenienceFuncs::splitString(s, ',');
  ASSERT_EQ(0, items.size());

  s = "xyz";
  items = ConvenienceFuncs::splitString(s, ',');
  ASSERT_EQ(1, items.size());
  ASSERT_EQ("xyz", items.at(0));
}

