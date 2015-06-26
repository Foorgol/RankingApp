
#include <Wt/WText>
#include <Wt/WContainerWidget>

#include "RankingApp.h"

RankingApp::RankingApp::RankingApp(const Wt::WEnvironment& env)
  :WApplication(env)
{
  setTitle("Hello World");
  root()->addWidget(new WText("Hey there!"));
}
