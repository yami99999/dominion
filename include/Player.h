#pragma once
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include "Card.h"

class Game;  // 前向声明 Game 类，避免在此头文件中引入 Game 类的完整定义，从而减少编译依赖。

class Player {
private:
    std::string name; // 玩家名称
    std::vector<std::shared_ptr<Card>> deck; //  玩家抽牌堆中的卡牌
    std::vector<std::shared_ptr<Card>> hand;  // 玩家手牌中的卡牌
    std::vector<std::shared_ptr<Card>> discard; // 玩家弃牌堆中的卡牌
    std::vector<std::shared_ptr<Card>> trash;  // 玩家废弃堆中的卡牌（用于某些特殊卡牌效果）。
    int actions; // 玩家当前的行动点数
    int buys; //  玩家当前的购买次数
    int coins; // 玩家当前的金币数
    
public:
    explicit Player(const std::string& name); // 显式构造函数，接受玩家名称作为参数，避免隐式转换。
    
    // 基础操作
    void drawCard(int count = 1); // 玩家从抽牌堆中抽取指定数量的卡牌，默认抽1张。
    void shuffleDeck(); // 洗牌玩家的抽牌堆，将弃牌堆中的卡牌重新混入抽牌堆。
    void discardHand(); // 玩家将手牌中的所有卡牌弃置到弃牌堆中。
    void initializeDeck(); // 初始化玩家的起始牌组（通常包括基础财宝卡和胜利点数卡）。
    void showHand() const; // 显示玩家当前手牌的内容。
    
    // 资源相关
    void addCoin(int amount) { coins += amount; } // 增加玩家的金币数
    void addAction(int amount) { actions += amount; } // 增加玩家的行动点数。
    void addBuy(int amount) { buys += amount; } //  增加玩家的购买次数。
    void spendCoins(int amount) { coins -= amount; } // 减少玩家的金币数。
    void useAction() { if (actions > 0) actions--; } // 使用一个行动点数，如果有剩余的话。
    void useBuy() { if (buys > 0) buys--; } //  使用一次购买次数，如果有剩余的话。
    
    // 获取状态
    int getActions() const { return actions; } // 获取当前玩家的行动点数。
    int getBuys() const { return buys; } // 获取当前玩家的购买次数。
    int getCoins() const { return coins; } //  获取当前玩家的金币数。
    std::string getName() const { return name; } // 获取玩家的名称。
    int getHandSize() const { return hand.size(); } // 获取玩家手牌的数量。
    
    // 卡片操作
    void addToDiscard(std::shared_ptr<Card> card); // 将一张卡牌添加到弃牌堆中。
    bool playAction(const std::string& cardName, Game& game); // 尝试使用一张指定名称的行动卡，传入 Game 实例以便执行卡牌效果。
    bool playTreasure(const std::string& cardName); // 尝试使用一张指定名称的财宝卡。
    void playAllTreasures(); // 自动使用玩家手牌中的所有财宝卡。
    
    // 特殊效果
    bool hasMoat() const;  // 检查是否有护城河, 用于防御攻击
    bool trashCopperFromHand();  //  尝试废弃手牌中的一张铜币卡牌，如果成功则返回 true。
    void discardDownTo(int count);  // 弃牌到指定数量
    int discardAndDraw();  // 让玩家弃掉任意数量的卡牌，然后再抽取相同数量的卡牌。

    
    // 获取卡片
    const std::vector<std::shared_ptr<Card>>& getHand() const { return hand; } // 获取玩家的手牌
    std::vector<std::shared_ptr<Card>> getTreasureCards() const; // 获取玩家手牌中的所有财宝卡。
    std::vector<std::shared_ptr<Card>> getAllCards() const;  // 获取玩家所有牌组中的卡牌（抽牌堆、手牌、弃牌堆）。
    
    // 存档相关
    void saveState(std::ofstream& out) const; // 将玩家的当前状态保存到输出文件流中。
    void loadState(std::ifstream& in); // 从输入文件流中加载玩家的状态
    
    bool hasActionCard() const;  // 检查玩家手牌中是否有行动卡
    void resetActions() { actions = 1; }  // 重置玩家的行动点数为1。
    void resetBuys() { buys = 1; }  // 重置玩家的购买次数为1
    void resetCoins() { coins = 0; }  // 重置玩家的金币数为0
    
    void discardCard(int index); // 弃掉手牌中指定索引的卡牌
    
    // 显示玩家抽牌堆和弃牌堆的状态
    void showDeckStatus() const;  // 显示牌堆状态
    int getDeckSize() const { return deck.size(); } // 获取玩家抽牌堆的大小
    int getDiscardSize() const { return discard.size(); }// 获取玩家弃牌堆的大小。


    void discardCellar();  // 弃掉玩家手牌中的一张地窖卡牌
    void showCardDescription(const std::string& cardName) const; // 显示指定卡牌的描述。
    


    // 添加的获取和设置方法
    const std::vector<std::shared_ptr<Card>>& getDeck() const { return deck; } // 获取玩家的抽牌堆。
    const std::vector<std::shared_ptr<Card>>& getDiscard() const { return discard; } //  获取玩家的弃牌堆。
    void setActions(int value) { actions = value; } //  设置玩家的行动点数。
    void setBuys(int value) { buys = value; } // 设置玩家的购买次数。
    void setCoins(int value) { coins = value; } // 设置玩家的金币数
    void addToDeck(std::shared_ptr<Card> card) { deck.push_back(card); } // 将一张卡牌添加到抽牌堆中。
    void addToHand(std::shared_ptr<Card> card) { hand.push_back(card); } // 将一张卡牌添加到手牌中。




    //////

     // 序列化玩家状态
    json toJson() const {
        json j;
        j["name"] = name;
        j["actions"] = actions;
        j["buys"] = buys;
        j["coins"] = coins;
        
        j["deck"] = json::array();
        for (const auto& card : deck) {
            j["deck"].push_back(card->toJson());
        }
        
        j["hand"] = json::array();
        for (const auto& card : hand) {
            j["hand"].push_back(card->toJson());
        }
        
        j["discard"] = json::array();
        for (const auto& card : discard) {
            j["discard"].push_back(card->toJson());
        }
        
        j["trash"] = json::array();
        for (const auto& card : trash) {
            j["trash"].push_back(card->toJson());
        }
        
        return j;
    }
    
    void fromJson(const json& j) {
        name = j["name"].get<std::string>();
        actions = j["actions"].get<int>();
        buys = j["buys"].get<int>();
        coins = j["coins"].get<int>();
        
        deck.clear();
        hand.clear();
        discard.clear();
        trash.clear();
        
        for (const auto& cardJson : j["deck"]) {
            deck.push_back(Card::fromJson(cardJson));
        }
        
        for (const auto& cardJson : j["hand"]) {
            hand.push_back(Card::fromJson(cardJson));
        }
        
        for (const auto& cardJson : j["discard"]) {
            discard.push_back(Card::fromJson(cardJson));
        }
        
        for (const auto& cardJson : j["trash"]) {
            trash.push_back(Card::fromJson(cardJson));
        }
    }
    
    // 添加获取废弃堆的方法
    const std::vector<std::shared_ptr<Card>>& getTrash() const { return trash; }
}; 