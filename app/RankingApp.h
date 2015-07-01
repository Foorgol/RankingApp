#ifndef RANKINGAPP_H
#define RANKINGAPP_H

#include <memory>

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>

#include "SqliteDatabase.h"

using namespace Wt;
using namespace SqliteOverlay;
using namespace std;

namespace RankingApp {

  class RankingApp : public WApplication
  {
  public:
    RankingApp(const WEnvironment& env);

  private:
    void createMenuBar();

    WStackedWidget* mainContainer;
  };

}

#endif /* RANKINGAPP_H */
