#ifndef CONVENIENCEFUNCS_H
#define CONVENIENCEFUNCS_H

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include "RankingErrCodes.h"

namespace RankingApp {

  class ConvenienceFuncs
  {
  public:
    static constexpr int MIN_YEAR = 1900;
    static constexpr int MAX_YEAR = 2050;  // arbitrarily chosen by me

    // trim from start
    static inline std::string &ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
            return s;
    }

    // trim from end
    static inline std::string &rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
            return s;
    }

    // trim from both ends
    static inline std::string &trim(std::string &s) {
            return ltrim(rtrim(s));
    }

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
