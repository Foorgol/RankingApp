#ifndef CONVENIENCEFUNCS_H
#define CONVENIENCEFUNCS_H

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>

#include "RankingErrCodes.h"

using namespace std;

namespace RankingApp {

  typedef vector<string> StringList;

  class ConvenienceFuncs
  {
  public:

    // assign an error value to a pointer
    static inline void setErr(ERR* ptr, ERR val)
    {
      if (ptr != nullptr)
      {
        *ptr = val;
      }
    }
  };

}

#endif  /* CONVENIENCEFUNCS_H */
