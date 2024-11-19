#pragma once
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <fstream>
#include <iomanip>
#include "Player.h"
#include "Utils.h"
#include "GameLogger.h"

class Card;  // 前向声明

class Game {
private:
    std::vector<Player> players;
    std::map<std::string, std::vector<std::shared_ptr<Card>>> supply;
    int currentPlayer;
    int turnCount;
    GameLogger logger;
    
    void displayResults(const std::vector<std::pair<std::string, int>>& scores);
    void displayStats();
    
public:
    explicit Game(int numPlayers = 0);
    
    void initialize();
    void playGame();
    void playTurn();
    bool isGameOver() const;
    
    void playActionPhase();
    void playBuyPhase();
    void playCleanupPhase();
    
    bool isProvinceEmpty() const;
    int getEmptyKingdomPiles() const;
    std::vector<std::pair<std::string, int>> calculateFinalScores() const;
    
    void saveGame(const std::string& filename);
    void loadGame(const std::string& filename);
    void logAction(const std::string& playerName, const std::string& action);
    
    void otherPlayersDiscardToThree(const Player& currentPlayer);
    void otherPlayersDraw(const Player& currentPlayer, int count);
    void gainCardUpToCost(Player& player, int maxCost);
}; 