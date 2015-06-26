#ifndef RANKINGAPP_H
#define RANKINGAPP_H

#include <Wt/WApplication>

#include "SqliteDatabase.h"

using namespace Wt;
using namespace SqliteOverlay;

namespace RankingApp {

  class RankingApp : public WApplication
  {
  public:
    RankingApp(const WEnvironment& env);
  };

}

#endif /* RANKINGAPP_H */
