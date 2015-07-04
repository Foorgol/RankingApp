#ifndef LAZYCONTENTLOADER_H
#define LAZYCONTENTLOADER_H

#include <memory>
#include <string>
#include <map>

#include <Wt/WStackedWidget>

#include "RankingSystem.h"

using namespace Wt;
using namespace SqliteOverlay;
using namespace std;

namespace RankingApp {

  class LazyContentLoader : public WStackedWidget
  {
  public:
    enum class CONTENT_TYPE {
      SINGLES_RANKING,
      DOUBLES_RANKING,
      ALL_PLAYERS,
      ERROR,
    };

  public:
    LazyContentLoader(RankingSystem* rs, WContainerWidget *parent=0);
    void switchContent(CONTENT_TYPE newContent);

  private:
    map<LazyContentLoader::CONTENT_TYPE, int> content2Index;
    RankingSystem* rankSys;
  };

}

#endif /* LAZYCONTENTLOADER_H */
