#include <Wt/WText>

#include "LazyContentLoader.h"

using namespace RankingApp;

LazyContentLoader::LazyContentLoader(WContainerWidget *parent)
  : WStackedWidget(parent)
{

}

//----------------------------------------------------------------------------

void LazyContentLoader::switchContent(LazyContentLoader::CONTENT_TYPE newContent)
{
  int* pIdx = nullptr;

  switch (newContent)
  {
  case CONTENT_TYPE::SINGLES_RANKING:
    pIdx = &singlesIdx;
    break;

  case CONTENT_TYPE::DOUBLES_RANKING:
    pIdx = &doublesIdx;
    break;

  case CONTENT_TYPE::ERROR:
    pIdx = &errorIdx;
    break;

  default:
    pIdx = nullptr;
  }

  if (pIdx == nullptr)
  {
    return;
  }

  // if the content has been accessed before,
  // switch over to the content
  if (*pIdx >= 0)
  {
    setCurrentIndex(*pIdx);
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

  case CONTENT_TYPE::ERROR:
    newWidget = new WText("Ups, da ist irgendein Fehler aufgetreten... :(");
    break;

  default:
    newWidget = new WText("LazyContentLoader: Error!");
  }

  // add the newly created widget to the stack
  addWidget(newWidget);
  setCurrentWidget(newWidget);
  *pIdx = currentIndex();
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

