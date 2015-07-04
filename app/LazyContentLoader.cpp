#include <Wt/WText>

#include "LazyContentLoader.h"
#include "AllPlayersWidget.h"

using namespace RankingApp;

LazyContentLoader::LazyContentLoader(RankingSystem* rs, WContainerWidget *parent)
  : WStackedWidget(parent), rankSys(rs)
{

}

//----------------------------------------------------------------------------

void LazyContentLoader::switchContent(LazyContentLoader::CONTENT_TYPE newContent)
{
  // check if we've already accessed the content before
  auto itContent = content2Index.find(newContent);
  if (itContent != content2Index.end())
  {
    setCurrentIndex((*itContent).second);
    return;
  }

  // the content has never been accessed before
  // and thus we need to create it
  WWidget* newWidget = nullptr;
  switch (newContent)
  {
  case CONTENT_TYPE::SINGLES_RANKING:
    newWidget = new WText("Einzel");
    break;

  case CONTENT_TYPE::DOUBLES_RANKING:
    newWidget = new WText("Doppel");
    break;

  case CONTENT_TYPE::ALL_PLAYERS:
    newWidget = new AllPlayersWidget(rankSys);
    break;

  case CONTENT_TYPE::ERROR:
    newWidget = new WText("Ups, da ist irgendein Fehler aufgetreten... :(");
    break;

  default:
    newWidget = new WText("LazyContentLoader: Error!");
  }

  // add the newly created widget to the stack
  addWidget(newWidget);
  setCurrentWidget(newWidget);
  int newIdx = currentIndex();
  content2Index[newContent] = newIdx;
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

