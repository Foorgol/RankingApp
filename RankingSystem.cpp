
#include "RankingSystem.h"
#include "RankingDb.h"
#include "ConvenienceFuncs.h"

using namespace RankingApp;

unique_ptr<RankingApp::RankingSystem> RankingApp::RankingSystem::get(const string& fname, RankingApp::ERR* err)
{
  return doInit(fname, false, err);
}

//----------------------------------------------------------------------------

unique_ptr<RankingSystem> RankingSystem::createEmpty(const string& fname, ERR* err)
{
  return doInit(fname, true, err);
}

//----------------------------------------------------------------------------

bool RankingApp::RankingSystem::isValidFilename(const string& fname)
{
  string fn = fname;
  ConvenienceFuncs::trim(fn);
  if (fn.empty()) return false;
  if (fn.front() == ':') return false;  // avoid special file names such as ":memory:"
  return true;
}

//----------------------------------------------------------------------------

PlayerMngr RankingSystem::getPlayerMngr() const
{
  return PlayerMngr(db.get());
}

//----------------------------------------------------------------------------

RankingSystem::RankingSystem(upRankingDb _db)
{
  // this checked should never trigger, because we only call
  // this function from get() with a pre-checked pointer.
  // But anyway...
  if (_db == nullptr)
  {
    throw std::invalid_argument("Received nullptr for database handle!");
  }

  // store the database handle
  db = std::move(_db);
}

//----------------------------------------------------------------------------

unique_ptr<RankingSystem> RankingSystem::doInit(const string& fname, bool doCreateNew, ERR* err)
{
  if (!(isValidFilename(fname)))
  {
    ConvenienceFuncs::setErr(err, ERR::INVALID_FILENAME);
    return nullptr;
  }

  // try to create a new database or open an existing
  string fn = fname;
  ConvenienceFuncs::trim(fn);
  auto dbPtr = SqliteDatabase::get<RankingDb>(fn, doCreateNew);

  // if that fails, return null
  if (dbPtr == nullptr)
  {
    if (doCreateNew)
    {
      ConvenienceFuncs::setErr(err, ERR::FILE_ALREADY_EXISTS);
    } else {
      ConvenienceFuncs::setErr(err, ERR::FILE_NOT_EXISTING_OR_INVALID);
    }
    return nullptr;
  }

  // create a new instance
  RankingSystem* tmpPtr = new RankingSystem(std::move(dbPtr));
  ConvenienceFuncs::setErr(err, ERR::SUCCESS);
  return unique_ptr<RankingSystem>(tmpPtr);
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
