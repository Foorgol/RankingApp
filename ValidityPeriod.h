#ifndef VALIDITYPERIOD_H
#define VALIDITYPERIOD_H

#include <memory>
#include <string>
#include <vector>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericDatabaseObject.h"
#include "GenericObjectManager.h"
#include "Sloppy/DateTime/DateAndTime.h"

using namespace SqliteOverlay;
using namespace boost::gregorian;

namespace RankingApp {

  class ValidityPeriod : public GenericDatabaseObject<RankingDb>
  {
    friend class PlayerMngr;
    friend class SqliteOverlay::GenericObjectManager<RankingDb>;

  public:
    static constexpr int IS_BEFORE_PERIOD = -1;
    static constexpr int IS_IN_PERIOD = 0;
    static constexpr int IS_AFTER_PERIOD = 1;

    bool hasEndDate() const;
    bool isInPeriod(const date& d) const;
    int determineRelationToPeriod(const date& d) const;

    // getters
    date getPeriodStart() const;
    unique_ptr<date> getPeriodEnd() const;

    // comparison functions for std::sort
    static std::function<bool (ValidityPeriod&, ValidityPeriod&)> getPlayerSortFunction_byActivationDate();

  private:
    ValidityPeriod(RankingDb* db, int rowId);
    ValidityPeriod(RankingDb* db, TabRow row);
  };

  typedef unique_ptr<ValidityPeriod> upValidity;
  typedef vector<ValidityPeriod> ValidityPeriodList;
}

#endif  /* VALIDITYPERIOD_H */
