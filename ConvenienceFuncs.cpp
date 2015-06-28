
#include "ConvenienceFuncs.h"



RankingApp::StringList& RankingApp::ConvenienceFuncs::splitString(const string& s, char delim, vector<string>& elems)
{
  stringstream ss(s);
  string item;

  while (getline(ss, item, delim))
  {
    elems.push_back(item);
  }

  return elems;
}

RankingApp::StringList RankingApp::ConvenienceFuncs::splitString(const string& s, char delim)
{
  std::vector<std::string> elems;
  splitString(s, delim, elems);
  return elems;
}
