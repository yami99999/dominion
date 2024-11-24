#pragma once
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include "Card.h"

class Game;  // 前向声明

class Player {
private:
    std::string name;
    std::vector<std::shared_ptr<Card>> deck;
    std::vector<std::shared_ptr<Card>> hand;
    std::vector<std::shared_ptr<Card>> discard;
    std::vector<std::shared_ptr<Card>> trash;
    int actions;
    int buys;
    int coins;
    
public:
    explicit Player(const std::string& name);
    
    // 基础操作
    void drawCard(int count = 1);
    void shuffleDeck();
    void discardHand();
    void initializeDeck();
    
    // 显示相关
    void showDeckStatus() const;
    void showHand() const;
    void showCardDescription(const std::string& cardName) const;
    
    // 资源管理
    void addAction(int amount) { actions += amount; }
    void addBuy(int amount) { buys += amount; }
    void addCoin(int amount) { coins += amount; }
    void useAction() { if (actions > 0) actions--; }
    void useBuy() { if (buys > 0) buys--; }
    void spendCoins(int amount) { coins -= amount; }
    void resetActions() { actions = 1; }
    void resetBuys() { buys = 1; }
    void resetCoins() { coins = 0; }
    void setActions(int value) { actions = value; }
    void setBuys(int value) { buys = value; }
    void setCoins(int value) { coins = value; }
    
    // 牌组操作
    void addToDiscard(std::shared_ptr<Card> card) { discard.push_back(card); }
    void addToDeck(std::shared_ptr<Card> card) { deck.push_back(card); }
    void addToHand(std::shared_ptr<Card> card) { hand.push_back(card); }
    void discardCard(int index);
    void discardCellar();
    void discardDownTo(int count);
    int discardAndDraw();
    bool trashCopperFromHand();
    void trashCardFromHand(size_t index);
    void trashCardFromDeck(size_t index);
    
    // 游戏动作
    bool playAction(const std::string& cardName, Game& game);
    bool playTreasure(const std::string& cardName);
    void playAllTreasures();
    bool hasActionCard() const;
    bool hasMoat() const;
    
    // 状态查询
    const std::string& getName() const { return name; }
    int getActions() const { return actions; }
    int getBuys() const { return buys; }
    int getCoins() const { return coins; }
    int getHandSize() const { return hand.size(); }
    const std::vector<std::shared_ptr<Card>>& getHand() const { return hand; }
    std::vector<std::shared_ptr<Card>>& getHand() { return hand; }
    const std::vector<std::shared_ptr<Card>>& getDeck() const { return deck; }
    std::vector<std::shared_ptr<Card>>& getDeck() { return deck; }
    const std::vector<std::shared_ptr<Card>>& getDiscard() const { return discard; }
    std::vector<std::shared_ptr<Card>> getAllCards() const;
    std::vector<std::shared_ptr<Card>> getTreasureCards() const;
    
    // 存档相关
    void saveState(std::ofstream& out) const;
    void loadState(std::ifstream& in);
    json toJson() const;
    void fromJson(const json& j);
}; 