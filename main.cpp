#include <iostream>

#include <Wt/WApplication>

#include "SqliteDatabase.h"
#include "RankingDb.h"
#include "app/RankingApp.h"

using namespace std;
using namespace SqliteOverlay;
using namespace Wt;

WApplication* createNewAppInstance(const WEnvironment& env)
{
  return new RankingApp::RankingApp(env, "uscNew.db");
}

int main(int argc, char **argv)
{
  // set the Wt's default locale to UTF-8 so that
  // std::strings are interpreted and displayed
  // correctly
  WString::setDefaultEncoding(UTF8);

  // run the main event / server loop
  return WRun(argc, argv, &createNewAppInstance);
}

