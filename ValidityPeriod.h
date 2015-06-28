#ifndef VALIDITYPERIOD_H
#define VALIDITYPERIOD_H

#include <memory>
#include <string>

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

  private:
    ValidityPeriod(SqliteDatabase* db, int rowId);
    ValidityPeriod(SqliteDatabase* db, TabRow row);
  };

  typedef unique_ptr<ValidityPeriod> upValidity;
}

#endif  /* VALIDITYPERIOD_H */
