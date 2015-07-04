#include <stdio.h>
#include <unistd.h>

#include <Wt/WText>
#include <Wt/WContainerWidget>
#include <Wt/WNavigationBar>
#include <Wt/WMenu>
#include <Wt/WMenuItem>
#include <Wt/WBootstrapTheme>
#include <Wt/WAnchor>
#include <Wt/WEnvironment>
#include <Wt/WString>

#include "RankingApp.h"
#include "RankingSystem.h"
#include "urls.h"

RankingApp::RankingApp::RankingApp(const Wt::WEnvironment& env, const string& dbPath)
  :WApplication(env)
{
  // try to access the database
  ERR err;
  rankSys = RankingSystem::get(dbPath, &err);
  if ((rankSys == nullptr) || (err != ERR::SUCCESS))
  {
    // get the current working dir for debugging purposes
    string curPath;
    char _curPath[FILENAME_MAX];
    if (!getcwd(_curPath, FILENAME_MAX))
    {
      curPath = "<unknown>";
    } else {
      curPath = string{_curPath};
    }

    // display nothing but an error message
    // if opening of the database fails
    string msg = "Konnte die Datenbank ";
    msg += dbPath;
    msg += " nicht öffnen. Info: app wird in ";
    msg += curPath;
    msg += " ausgeführt";
    root()->addWidget(new WText(msg));

    return;
  }

  // initialize the rest of the web app if we could
  // successfully open the database
  WBootstrapTheme* theme = new WBootstrapTheme();
  theme->setVersion(WBootstrapTheme::Version3);
  setTheme(theme);
  setTitle("Hello World - " + env.deploymentPath());
  createMenuBar();


}

//----------------------------------------------------------------------------

void RankingApp::RankingApp::createMenuBar()
{
  WContainerWidget* c = new WContainerWidget();

  // the navigation bar itself
  WNavigationBar* navBar = new WNavigationBar(c);
  navBar->setTitle("USC Badminton", "http://www.google.com");
  navBar->setResponsive(true);

  // the container for the main content of the screen;
  // the content is switched by different menu selections
  mainContainer = new WStackedWidget(c);
  mainContainer->setStyleClass("contents");

  // a helper function for adding a text item linked to
  // an internal URL to a menu
  auto addLinkItem = [](WMenu* m, const string& label, const string& link) {
    WMenuItem* mi = new WMenuItem(label);
    mi->setLink(Wt::WLink(Wt::WLink::InternalPath, link));
    m->addItem(mi);
  };

  // create the menu on the left
  WMenu* leftMenu = new WMenu();
  navBar->addMenu(leftMenu);
  leftMenu->setInternalPathEnabled(BASE_URL);
  addLinkItem(leftMenu, "Einzel", SINGLES_URL);
  addLinkItem(leftMenu, "Doppel", DOUBLES_URL);

  // create a sub-menu for various info-elements
  WMenu* infoMenu = new WMenu(mainContainer, c);
  addLinkItem(infoMenu, "Alle Spieler", ALL_PLAYERS_URL);

  // add the info menu to the left menu
  leftMenu->addMenu("Infos", infoMenu);

  c->addWidget(mainContainer);

  root()->addWidget(c);
}
