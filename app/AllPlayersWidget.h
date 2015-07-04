#ifndef ALLPLAYERSWIDGET_H
#define ALLPLAYERSWIDGET_H

#include <memory>
#include <string>

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WTable>

#include "RankingSystem.h"

using namespace Wt;
using namespace SqliteOverlay;
using namespace std;

namespace RankingApp {

  class AllPlayersWidget : public WContainerWidget
  {
  public:
    AllPlayersWidget(RankingSystem* rs, WContainerWidget* parent=nullptr);

  private:
    RankingSystem* rankSys;
    WTable* playerTab;

    void fillTable();


  };

}

#endif /* ALLPLAYERSWIDGET_H */
