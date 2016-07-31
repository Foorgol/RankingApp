#include <memory>
#include <vector>
#include <exception>

#include <boost/algorithm/string.hpp>


#include "Score.h"
#include "ConvenienceFuncs.h"


namespace RankingApp {

bool GameScore::isValidScore(int sc1, int sc2)
{
  // no negative scores
  if ((sc1 < 0) || (sc2 < 0)) return false;

  // no score above 30
  if ((sc1 > 30) || (sc2 > 30)) return false;

  // no draw
  if (sc1 == sc2) return false;

  int maxScore = (sc1 > sc2) ? sc1 : sc2;
  int minScore = (maxScore == sc1) ? sc2 : sc1;

  // special case: victory by 30:29
  if ((maxScore == 30) && (minScore == 29)) return true;

  // any other case:
  // maxScore must be at least 21 and minScore two below
  if (maxScore < 21) return false;
  if ((maxScore - minScore) < 2) return false;
  if ((maxScore > 21) && ((maxScore - minScore) != 2)) return false;

  return true;
}

//----------------------------------------------------------------------------

unique_ptr<GameScore> GameScore::fromScore(int sc1, int sc2)
{
  // allow only valid scores to be instanciated as a new GameScore object
  if (!(isValidScore(sc1, sc2))) return nullptr;

  // create and return new object
  return unique_ptr<GameScore>(new GameScore(sc1, sc2));
}

//----------------------------------------------------------------------------

GameScore::GameScore(int sc1, int sc2)
{
  if (!(isValidScore(sc1, sc2)))
  {
    // should never happen since the ctor is private and the
    // factory methods fromString and fromScore make all
    // necessary checks
    throw std::runtime_error("Attempt to create GameScore object with invalid score");
  }

  player1Score = sc1;
  player2Score = sc2;
}

//----------------------------------------------------------------------------

unique_ptr<GameScore> GameScore::fromString(const string &s)
{
  // we need exactly one delimiter
  StringList scores;
  boost::split(scores, s, boost::is_any_of(":"));
  if (scores.size() != 2) return nullptr;

  // both scores must be valid numbers
  int sc1 = -1;
  int sc2 = -1;
  try
  {
    string sc = scores.at(0);
    boost::trim(sc);
    sc1 = stoi(sc);

    sc = scores.at(1);
    boost::trim(sc);
    sc2 = stoi(sc);
  }
  catch (exception ex) {
    return nullptr;
  }

  return fromScore(sc1, sc2);
}

//----------------------------------------------------------------------------

string GameScore::toString() const
{
  return to_string(player1Score) + ":" + to_string(player2Score);
}

//----------------------------------------------------------------------------

tuple<int, int> GameScore::getScore() const
{
  return make_tuple(player1Score, player2Score);
}

//----------------------------------------------------------------------------

int GameScore::getWinner() const
{
  return (player1Score > player2Score) ? 1 : 2;
}

//----------------------------------------------------------------------------

int GameScore::getLoser() const
{
  return (player1Score < player2Score) ? 1 : 2;
}

//----------------------------------------------------------------------------

int GameScore::getWinnerScore() const
{
  return (player1Score > player2Score) ? player1Score : player2Score;
}

//----------------------------------------------------------------------------

int GameScore::getLoserScore() const
{
  return (player1Score < player2Score) ? player1Score : player2Score;
}

//----------------------------------------------------------------------------

void GameScore::swapPlayers()
{
  int tmp = player1Score;
  player1Score = player2Score;
  player2Score = tmp;
}

//----------------------------------------------------------------------------

int GameScore::getWinnerScoreForLoserScore(int loserScore)
{
  if (loserScore < 0) return -1; // invalid value
  if (loserScore < 20) return 21;
  if (loserScore < 29) return loserScore + 2;
  if (loserScore == 29) return 30;
  return -1;  // invalid
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
//----------------MatchScore class--------------------------------------------
//----------------------------------------------------------------------------

GameScoreList MatchScore::string2GameScoreList(const string& s)
{
  GameScoreList result;

  // games are comma-separated
  StringList games;
  boost::split(games, s, boost::is_any_of(","));
  for (string gameString : games)
  {
    auto gameSore = GameScore::fromString(gameString);

    // on error return an empty list
    if (gameSore == nullptr)
    {
      return GameScoreList();
    }

    result.push_back(*gameSore);
  }

  return result;
}

//----------------------------------------------------------------------------

bool MatchScore::isValidScore(const string& s)
{
  GameScoreList gsl = MatchScore::string2GameScoreList(s);
  return isValidScore(gsl);
}

//----------------------------------------------------------------------------

bool MatchScore::isValidScore(const GameScoreList& gsl)
{
  // empty list (e.g., from invalid string)
  if (gsl.empty()) return false;

  // get statistics about wins and losses of the two players
  auto gameSum = getGameSum(gsl);
  int p1Wins = get<0>(gameSum);
  int p2Wins = get<1>(gameSum);

  // check for draw
  if (p1Wins == p2Wins)
  {
    return false;
  }

  // in a regular match, the sum of all games may not
  // exceed three
  if (gsl.size() > 3) return false;

  // player one must have either won the necessary number
  // of games or lost the necessary number of games (which
  // implicitly means that player 2 won the games)
  if ((p1Wins == 2) || (p2Wins == 2)) return true;

  // catch any other error
  return false;
}

//----------------------------------------------------------------------------

unique_ptr<MatchScore> MatchScore::fromString(const string& s)
{
  GameScoreList gsl = string2GameScoreList(s);

  return fromGameScoreList(gsl);
}

//----------------------------------------------------------------------------

unique_ptr<MatchScore> MatchScore::fromStringWithoutValidation(const string& s)
{
  GameScoreList gsl = string2GameScoreList(s);

  return fromGameScoreListWithoutValidation(gsl);
}

//----------------------------------------------------------------------------

unique_ptr<MatchScore> MatchScore::fromGameScoreList(const GameScoreList& gsl)
{
  if (!(isValidScore(gsl)))
  {
    return nullptr;
  }

  return fromGameScoreListWithoutValidation(gsl);
}

//----------------------------------------------------------------------------

unique_ptr<MatchScore> MatchScore::fromGameScoreListWithoutValidation(const GameScoreList& gsl)
{
  MatchScore* result = new MatchScore();
  for (GameScore game : gsl)
  {
    result->addGame(game);
  }

  return unique_ptr<MatchScore>(result);
}

//----------------------------------------------------------------------------

bool MatchScore::addGame(const GameScore& sc)
{
  games.push_back(sc);
  return true;
}

//----------------------------------------------------------------------------

bool MatchScore::addGame(const string& scString)
{
  auto sc = GameScore::fromString(scString);
  if (sc == nullptr) return false;

  games.push_back(*sc);
  return true;
}

//----------------------------------------------------------------------------

string MatchScore::toString() const
{
  if (games.empty()) return "";

  string result;
  for (GameScore g : games)
  {
    if (!(result.empty()))
    {
      result += ",";
    }
    result += g.toString();
  }

  return result;
}

//----------------------------------------------------------------------------

int MatchScore::getWinner() const
{
  auto gamesSum = getGameSum();
  int p1Games = get<0>(gamesSum);
  int p2Games = get<1>(gamesSum);
  if (p1Games > p2Games) return 1;
  if (p2Games > p1Games) return 2;
  return 0; // draw
}

//----------------------------------------------------------------------------

int MatchScore::getLoser() const
{
  int winner = getWinner();
  if (winner == 1) return 2;
  if (winner == 2) return 1;
  return 0;  // draw
}

//----------------------------------------------------------------------------

tuple<int, int> MatchScore::getGameSum() const
{
  return getGameSum(games);
}

//----------------------------------------------------------------------------

tuple<int, int> MatchScore::getGameSum(const GameScoreList& gsl)
{
  // get statistics about wins and losses of the two players
  int p1Wins = 0;
  for (GameScore game : gsl)
  {
    if (game.getWinner() == 1) ++p1Wins;
  }
  int p2Wins = gsl.size() - p1Wins;

  return make_tuple(p1Wins, p2Wins);
}

//----------------------------------------------------------------------------

tuple<int, int> MatchScore::getMatchSum() const
{
  if (getWinner() == 1) return make_tuple(1, 0);
  if (getWinner() == 2) return make_tuple(0, 1);
  return make_tuple(0,0); //draw
}

//----------------------------------------------------------------------------

void MatchScore::swapPlayers()
{
  for (GameScore& game : games)
  {
    game.swapPlayers();
  }
}

//----------------------------------------------------------------------------

tuple<int, int> MatchScore::getScoreSum() const
{
  int p1Score = 0;
  int p2Score = 0;
  for (GameScore game : games)
  {
    auto sc = game.getScore();
    p1Score += get<0>(sc);
    p2Score += get<1>(sc);
  }

  return make_tuple(p1Score, p2Score);
}

//----------------------------------------------------------------------------

bool MatchScore::isValidScore() const
{
  return isValidScore(games);
}

//----------------------------------------------------------------------------

unique_ptr<GameScore> MatchScore::getGame(int n) const
{
  if ((n < 0) || (n > (games.size() - 1)))
  {
    return nullptr;
  }

  return unique_ptr<GameScore>(new GameScore(games.at(n)));
}

//----------------------------------------------------------------------------

int MatchScore::getNumGames() const
{
  return games.size();
}

//----------------------------------------------------------------------------

int MatchScore::getPointsSum() const
{
  auto scoreSum = getScoreSum();
  return (get<0>(scoreSum) + get<1>(scoreSum));
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

}
