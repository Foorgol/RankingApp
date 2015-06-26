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
  return new RankingApp::RankingApp(env);
}

int main(int argc, char **argv)
{
  return WRun(argc, argv, &createNewAppInstance);
}

