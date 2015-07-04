#ifndef LAZYCONTENTLOADER_H
#define LAZYCONTENTLOADER_H

#include <memory>
#include <string>

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
      ERROR,
    };

  public:
    LazyContentLoader(WContainerWidget *parent=0);
    void switchContent(CONTENT_TYPE newContent);

  private:
    int singlesIdx = -1;
    int doublesIdx = -1;
    int errorIdx = -1;
  };

}

#endif /* LAZYCONTENTLOADER_H */
