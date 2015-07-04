#include <Wt/WText>
#include <Wt/WString>
#include <Wt/WApplication>

#include "AllPlayersWidget.h"
#include "ConvenienceFuncs.h"


RankingApp::AllPlayersWidget::AllPlayersWidget(RankingApp::RankingSystem* rs, WContainerWidget* parent)
  :WContainerWidget(parent), rankSys(rs)
{
  // create a table for all players
  playerTab = new WTable(this);
  fillTable();

}

//----------------------------------------------------------------------------

void RankingApp::AllPlayersWidget::fillTable()
{
  playerTab->clear();

  // set the table headers
  playerTab->setHeaderCount(1);
  int colId = 0;
  for (string colTitle : {"Nr.", "Name", "Erste Aktivierung", "Letzte Deaktivierung"})
  {
    playerTab->elementAt(0, colId)->addWidget(new Wt::WText(colTitle));
    ++colId;
  }

  // fill the table
  PlayerMngr pm = rankSys->getPlayerMngr();
  int row = 1;
  PlayerList pList = pm.getAllPlayers();
  sort(pList.begin(), pList.end(), PlayerMngr::getPlayerSortFunction_byLastName());
  for (Player p : pList)
  {
    playerTab->elementAt(row, 0)->addWidget(new Wt::WText(to_string(row)));
    playerTab->elementAt(row, 1)->addWidget(new Wt::WText(p.getName__LastNameFirst()));

    string actDate_String;
    auto actDate = pm.getEarliestActivationDateForPlayer(p);
    if (actDate != nullptr)
    {
      actDate_String = actDate->getISODate();
    }
    playerTab->elementAt(row, 2)->addWidget(new Wt::WText(actDate_String));

    string deactDate_String;
    auto deactDate = pm.getLatestDeactivationDateForPlayer(p);
    if (deactDate != nullptr)
    {
      deactDate_String = deactDate->getISODate();
    }
    playerTab->elementAt(row, 3)->addWidget(new Wt::WText(deactDate_String));

    ++row;
  }

  playerTab->addStyleClass("table form-inline table-hover table-striped");
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

