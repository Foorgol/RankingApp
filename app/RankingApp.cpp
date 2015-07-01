
#include <Wt/WText>
#include <Wt/WContainerWidget>
#include <Wt/WNavigationBar>
#include <Wt/WMenu>
#include <Wt/WBootstrapTheme>
#include <Wt/WAnchor>

#include "RankingApp.h"

RankingApp::RankingApp::RankingApp(const Wt::WEnvironment& env)
  :WApplication(env)
{
  WBootstrapTheme* theme = new WBootstrapTheme();
  theme->setVersion(WBootstrapTheme::Version3);
  setTheme(theme);
  setTitle("Hello World");
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
