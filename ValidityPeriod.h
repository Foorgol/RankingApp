#ifndef VALIDITYPERIOD_H
#define VALIDITYPERIOD_H

#include <memory>
#include <string>
#include <vector>

#include "RankingDb.h"
#include "RankingErrCodes.h"
#include "GenericDatabaseObject.h"
#include "GenericObjectManager.h"
#include "DateAndTime.h"

using namespace SqliteOverlay;

namespace RankingApp {

  class ValidityPeriod : public GenericDatabaseObject
  {
    friend class PlayerMngr;
    friend class SqliteOverlay::GenericObjectManager;

  public:
    static constexpr int IS_BEFORE_PERIOD = -1;
    static constexpr int IS_IN_PERIOD = 0;
    static constexpr int IS_AFTER_PERIOD = 1;

    bool hasEndDate() const;
    bool isInPeriod(const LocalTimestamp& lt) const;
    int determineRelationToPeriod(const LocalTimestamp& lt) const;

    // getters
    upLocalTimestamp getPeriodStart() const;
    upLocalTimestamp getPeriodEnd() const;

    // comparison functions for std::sort
    static std::function<bool (ValidityPeriod&, ValidityPeriod&)> getPlayerSortFunction_byActivationDate();

  private:
    ValidityPeriod(SqliteDatabase* db, int rowId);
    ValidityPeriod(SqliteDatabase* db, TabRow row);

    time_t getRawBeginTime() const;
    time_t getRawEndTime() const;
  };

  typedef unique_ptr<ValidityPeriod> upValidity;
  typedef vector<ValidityPeriod> ValidityPeriodList;
}

#endif  /* VALIDITYPERIOD_H */
