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
    static constexpr int MIN_YEAR = 1900;
    static constexpr int MAX_YEAR = 2050;  // arbitrarily chosen by me

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
