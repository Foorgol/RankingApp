#include <stdio.h>
#include <unistd.h>

#include <Wt/WText>
#include <Wt/WContainerWidget>
#include <Wt/WNavigationBar>
#include <Wt/WMenu>
#include <Wt/WBootstrapTheme>
#include <Wt/WAnchor>
#include <Wt/WEnvironment>
#include <Wt/WString>

#include "RankingApp.h"
#include "RankingSystem.h"

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

  // the menu on the left
  WMenu* leftMenu = new WMenu(mainContainer, c);
  navBar->addMenu(leftMenu);
  leftMenu->setInternalPathEnabled("/basePath");
  leftMenu->addItem("Einzel", new WText("Einzel-RL"));
  leftMenu->addItem("Doppel", new WText("Doppel-RL"));

  c->addWidget(mainContainer);

  root()->addWidget(c);
}
