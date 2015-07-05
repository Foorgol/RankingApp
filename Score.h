#ifndef SCORE_H
#define SCORE_H

#include <memory>
#include <tuple>
#include <string>
#include <vector>


using namespace std;

namespace RankingApp {

class GameScore
{
public:
  static bool isValidScore(int sc1, int sc2);
  static unique_ptr<GameScore> fromScore(int sc1, int sc2);
  static unique_ptr<GameScore> fromString(const string& s);
  static int getWinnerScoreForLoserScore(int loserScore);

  string toString() const;

  tuple<int, int> getScore() const;
  int getWinner() const;
  int getLoser() const;
  int getWinnerScore() const;
  int getLoserScore() const;
  void swapPlayers();

private:
  GameScore(int sc1, int sc2);

  int player1Score;
  int player2Score;

};

typedef vector<GameScore> GameScoreList;

class MatchScore
{
public:
  static unique_ptr<MatchScore> fromString(const string& s);
  static unique_ptr<MatchScore> fromStringWithoutValidation(const string& s);
  static unique_ptr<MatchScore> fromGameScoreList(const GameScoreList& gsl);
  static unique_ptr<MatchScore> fromGameScoreListWithoutValidation(const GameScoreList& gsl);
  static bool isValidScore(const string& s);
  static bool isValidScore(const GameScoreList& gsl);
  bool isValidScore() const;

  string toString() const;

  int getWinner() const;
  int getLoser() const;

  int getNumGames() const;
  int getPointsSum() const;

  unique_ptr<GameScore> getGame(int n) const;

  tuple<int, int> getScoreSum() const;

  tuple<int, int> getGameSum() const;
  static tuple<int, int> getGameSum(const GameScoreList& gsl);

  tuple<int, int> getMatchSum() const;

  static unique_ptr<MatchScore> genRandomScore(int numWinGames=2, bool drawAllowed=false);

  void swapPlayers();

private:
  MatchScore() {}
  bool addGame(const GameScore& sc);
  bool addGame(const string& scString);
  static GameScoreList string2GameScoreList(const string& s);

  GameScoreList games;

};

typedef vector<MatchScore> MatchScoreList;

}
#endif // SCORE_H
