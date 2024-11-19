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
    std::vector<std::shared_ptr<Card>> trash;  // 添加废弃堆
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
    void showHand() const;
    
    // 资源相关
    void addCoin(int amount) { coins += amount; }
    void addAction(int amount) { actions += amount; }
    void addBuy(int amount) { buys += amount; }
    void spendCoins(int amount) { coins -= amount; }
    void useAction() { if (actions > 0) actions--; }
    void useBuy() { if (buys > 0) buys--; }
    
    // 获取状态
    int getActions() const { return actions; }
    int getBuys() const { return buys; }
    int getCoins() const { return coins; }
    std::string getName() const { return name; }
    int getHandSize() const { return hand.size(); }
    
    // 卡片操作
    void addToDiscard(std::shared_ptr<Card> card);
    bool playAction(const std::string& cardName, Game& game);
    bool playTreasure(const std::string& cardName);
    void playAllTreasures();
    
    // 特殊效果
    bool hasMoat() const;  // 检查是否有护城河
    bool trashCopperFromHand();  // 废弃一张铜币
    void discardDownTo(int count);  // 弃牌到指定数量
    int discardAndDraw();  // 弃牌并抽相同数量的牌
    
    // 获取卡片
    const std::vector<std::shared_ptr<Card>>& getHand() const { return hand; }
    std::vector<std::shared_ptr<Card>> getTreasureCards() const;
    std::vector<std::shared_ptr<Card>> getAllCards() const;
    
    // 存档相关
    void saveState(std::ofstream& out) const;
    void loadState(std::ifstream& in);
    
    bool hasActionCard() const;  // 添加这行
    void resetActions() { actions = 1; }  // 添加这行
    void resetBuys() { buys = 1; }  // 添加这行
    void resetCoins() { coins = 0; }  // 添加这行
    
    void discardCard(int index);
    
    // 添加新的显示方法
    void showDeckStatus() const;  // 显示牌堆状态
    int getDeckSize() const { return deck.size(); }
    int getDiscardSize() const { return discard.size(); }
    
    void discardCellar();  // 添加这个方法声明
}; 