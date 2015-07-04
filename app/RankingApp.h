#ifndef RANKINGAPP_H
#define RANKINGAPP_H

#include <memory>
#include <string>

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>

#include "SqliteDatabase.h"
#include "RankingSystem.h"
#include "LazyContentLoader.h"

using namespace Wt;
using namespace SqliteOverlay;
using namespace std;

namespace RankingApp {

  class RankingApp : public WApplication
  {
  public:
    RankingApp(const WEnvironment& env, const string& dbPath);

  private:
    upRankingSystem rankSys;

    void createMenuBar();
    void onInternalPathChanged();

    LazyContentLoader* mainContainer;
  };

}

#endif /* RANKINGAPP_H */
