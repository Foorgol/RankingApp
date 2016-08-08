
#include "ValidityPeriod.h"
#include "RankingDb.h"
#include "ConvenienceFuncs.h"
#include "RankingDataDefs.h"
#include "TabRow.h"

using namespace RankingApp;

constexpr int ValidityPeriod::IS_BEFORE_PERIOD;
constexpr int ValidityPeriod::IS_IN_PERIOD;
constexpr int ValidityPeriod::IS_AFTER_PERIOD;

ValidityPeriod::ValidityPeriod(RankingDb* db, int rowId)
:GenericDatabaseObject(db, TAB_PLAYER, rowId)
{
}

//----------------------------------------------------------------------------

ValidityPeriod::ValidityPeriod(RankingDb* db, TabRow row)
:GenericDatabaseObject(db, row)
{
}

//----------------------------------------------------------------------------

bool ValidityPeriod::hasEndDate() const
{
  auto end = row.getDate2(VA_PERIOD_END);
  return !(end->isNull());
}

//----------------------------------------------------------------------------

bool ValidityPeriod::isInPeriod(const date& d) const
{
  return (determineRelationToPeriod(d) == IS_IN_PERIOD);
}

//----------------------------------------------------------------------------

int ValidityPeriod::determineRelationToPeriod(const date& d) const
{
  date start = row.getDate(VA_PERIOD_START);
  if (d < start)
  {
    return IS_BEFORE_PERIOD;
  }

  auto _endDate = row.getDate2(VA_PERIOD_END);
  if (_endDate->isNull())
  {
    // no end time set, so we are "per definitionem"
    // within the time period
    return IS_IN_PERIOD;
  }

  date end = _endDate->get();
  return (d <= end) ? IS_IN_PERIOD : IS_AFTER_PERIOD;
}

//----------------------------------------------------------------------------

date ValidityPeriod::getPeriodStart() const
{
  return row.getDate(VA_PERIOD_START);
}

//----------------------------------------------------------------------------

unique_ptr<date> ValidityPeriod::getPeriodEnd() const
{
  auto end = row.getDate2(VA_PERIOD_END);
  if (end->isNull()) return nullptr;

  return unique_ptr<date>(new date(end->get()));
}

//----------------------------------------------------------------------------

std::function<bool (ValidityPeriod&, ValidityPeriod&)> ValidityPeriod::getPlayerSortFunction_byActivationDate()
{
  return [](ValidityPeriod& v1, ValidityPeriod& v2) {
    return (v1.getPeriodStart() < v2.getPeriodStart());
  };
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
