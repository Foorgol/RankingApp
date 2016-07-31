
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

time_t ValidityPeriod::getRawBeginTime() const
{
  auto timestamp = row.getUTCTime2(VA_PERIOD_START);
  return (timestamp->isNull()) ? -1 : timestamp->get().getRawTime();
}

//----------------------------------------------------------------------------

time_t ValidityPeriod::getRawEndTime() const
{
  auto timestamp = row.getUTCTime2(VA_PERIOD_END);
  return (timestamp->isNull()) ? -1 : timestamp->get().getRawTime();
}

//----------------------------------------------------------------------------

bool ValidityPeriod::hasEndDate() const
{
  auto timestamp = row.getLocalTime2(VA_PERIOD_END);
  return !(timestamp->isNull());
}

//----------------------------------------------------------------------------

bool ValidityPeriod::isInPeriod(const LocalTimestamp& lt) const
{
  return (determineRelationToPeriod(lt) == IS_IN_PERIOD);
}

//----------------------------------------------------------------------------

int ValidityPeriod::determineRelationToPeriod(const LocalTimestamp& lt) const
{
  LocalTimestamp startTime = row.getLocalTime(VA_PERIOD_START);
  if (lt < startTime)
  {
    return IS_BEFORE_PERIOD;
  }

  auto _endTime = row.getLocalTime2(VA_PERIOD_END);
  if (_endTime->isNull())
  {
    // no end time set, so we are "per definitionem"
    // within the time period
    return IS_IN_PERIOD;
  }

  LocalTimestamp endTime = _endTime->get();
  return (lt <= endTime) ? IS_IN_PERIOD : IS_AFTER_PERIOD;
}

//----------------------------------------------------------------------------

upLocalTimestamp ValidityPeriod::getPeriodStart() const
{
  auto timestamp = row.getLocalTime2(VA_PERIOD_START);
  if (timestamp->isNull()) return nullptr;

  return upLocalTimestamp(new LocalTimestamp(timestamp->get().getRawTime()));
}

//----------------------------------------------------------------------------

upLocalTimestamp ValidityPeriod::getPeriodEnd() const
{
  auto timestamp = row.getLocalTime2(VA_PERIOD_END);
  if (timestamp->isNull()) return nullptr;

  return upLocalTimestamp(new LocalTimestamp(timestamp->get().getRawTime()));
}

//----------------------------------------------------------------------------

std::function<bool (ValidityPeriod&, ValidityPeriod&)> ValidityPeriod::getPlayerSortFunction_byActivationDate()
{
  return [](ValidityPeriod& v1, ValidityPeriod& v2) {
    return (v1.getRawBeginTime() < v2.getRawBeginTime());
  };
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
