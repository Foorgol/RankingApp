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
#include <Wt/WApplication>

#include "RankingApp.h"
#include "RankingSystem.h"
#include "urls.h"
#include "ConvenienceFuncs.h"

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

  //
  // initialize the rest of the web app if we could
  // successfully open the database
  //

  // set theme and title
  WBootstrapTheme* theme = new WBootstrapTheme();
  theme->setVersion(WBootstrapTheme::Version3);
  setTheme(theme);
  setTitle("USC Rangliste");

  // intialize the container for the main display content
  mainContainer = new LazyContentLoader();
  mainContainer->setStyleClass("contents");

  // create the menu
  createMenuBar();

  // add the main container to the root window
  root()->addWidget(mainContainer);

  // connect the handler for internal path changes
  WApplication *app = WApplication::instance();
  app->internalPathChanged().connect(std::bind([=] () {
    onInternalPathChanged();
  }));

  // initialize / show the default content
  onInternalPathChanged();
}

//----------------------------------------------------------------------------

void RankingApp::RankingApp::createMenuBar()
{
  WContainerWidget* c = new WContainerWidget();

  // the navigation bar itself
  WNavigationBar* navBar = new WNavigationBar(c);
  navBar->setTitle("USC Badminton", "http://www.google.com");
  navBar->setResponsive(true);

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
  WMenu* infoMenu = new WMenu();
  infoMenu->setInternalPathEnabled(BASE_URL);
  addLinkItem(infoMenu, "Alle Spieler", ALL_PLAYERS_URL);

  // add the info menu to the left menu
  leftMenu->addMenu("Infos", infoMenu);

  root()->addWidget(c);
}

//----------------------------------------------------------------------------

void RankingApp::RankingApp::onInternalPathChanged()
{
  WApplication *app = WApplication::instance();
  string fullPath = app->internalPath();

  // an ugly method to get only the last path component
  StringList pathComp = ConvenienceFuncs::splitString(fullPath, '/');
  string pageName = pathComp.at(pathComp.size() - 1);

  LazyContentLoader::CONTENT_TYPE newContent = LazyContentLoader::CONTENT_TYPE::ERROR;
  if (pageName == SINGLES_URL)
  {
    newContent = LazyContentLoader::CONTENT_TYPE::SINGLES_RANKING;
  }
  if (pageName == DOUBLES_URL)
  {
    newContent = LazyContentLoader::CONTENT_TYPE::DOUBLES_RANKING;
  }

  mainContainer->switchContent(newContent);
}
