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

class Card;  // 前向声明 déclaration à terme

class Game {
private:
    std::vector<Player> players;
    std::map<std::string, std::vector<std::shared_ptr<Card>>> supply;
    int currentPlayer;
    int turnCount;
    GameLogger logger;
    
    std::vector<std::string> manualSelectCards();
    std::vector<std::string> selectPresetCards();
    void viewCardDetails();
    void displayKingdomCardInfo(const std::string& cardName) const;
    void displayGameState() const;
    void displayCurrentPlayerStatus() const;
    
    void displayResults(const std::vector<std::pair<std::string, int>>& scores);
    void displayStats();
    void displayFinalState() const;
    
    void initializeKingdomCards(const std::vector<std::string>& selectedCards);
    std::vector<std::string> selectKingdomCards();
    std::vector<std::string> getRandomKingdomCards(int count = 10);
    void displayAvailableKingdomCards() const;
    
    void initialize(const std::vector<std::string>& selectedCards);
    std::vector<std::string> getPresetKingdomCards(const std::string& setName);
    
public:
    explicit Game(int numPlayers, bool randomKingdom = false, 
                 const std::string& kingdomSet = "");
    
    void initialize();
    void playGame();
    void playTurn();
    bool isGameOver() const;
    
    void playActionPhase();
    void playBuyPhase();
    void playCleanupPhase();
    
    bool isProvinceEmpty() const;
    int getEmptyPiles() const;
    std::vector<std::pair<std::string, int>> calculateFinalScores() const;
    
    void saveGame(const std::string& filename);
    void loadGame(const std::string& filename);
    void logAction(const std::string& playerName, const std::string& action);
    
    void otherPlayersDiscardToThree(const Player& currentPlayer);
    void otherPlayersDraw(const Player& currentPlayer, int count);
    void gainCardUpToCost(Player& player, int maxCost);
    
    void showSupplyCardDescription(const std::string& cardName) const;
    void displaySupplyPiles() const;
    
    std::string getCurrentPlayerName() const {
        return players[currentPlayer].getName();
    }
    
    int getCurrentPlayerActions() const {
        return players[currentPlayer].getActions();
    }
    
    int getCurrentPlayerBuys() const {
        return players[currentPlayer].getBuys();
    }
    
    int getCurrentPlayerCoins() const {
        return players[currentPlayer].getCoins();
    }
    
    void otherPlayersGainCurse(const Player& currentPlayer);
    void resolveThiefEffect(Player& currentPlayer);
    void resolveChapelEffect(Player& currentPlayer);
    int calculateGardensPoints(const Player& player) const;
    
    static const std::vector<std::string> ALL_KINGDOM_CARDS;
    
    void displaySupplyStatus() const;
    void displayPlayerStatus(const Player& player) const;
    std::string getKingdomCardSetName() const;
}; 