#pragma once
#include <string> // std::string 用于处理卡牌名称和描述。
#include <memory> // 用于智能指针管理卡牌对象的生命周期
#include <fstream> // 引入 Player 和 Game 类的定义，允许卡牌在使用时与玩家和游戏状态交互。
#include <nlohmann/json.hpp> // json处理




// 前向声明
class Player;
class Game;

// 使用 nlohmann::json
using json = nlohmann::json;


// 定义卡牌类型的枚举类
enum class CardType {
    TREASURE, // 财宝卡，如铜币、银币、金币
    VICTORY, //  胜利点数卡，如庄园、领地、行省。
    ACTION, // 行动卡，如村庄、地窖等
    CURSE //  诅咒卡，对玩家产生负面效果。?
};

class Card {
protected:
    std::string name; // 卡牌名称。
    int cost; // 卡牌花费
    CardType type; // 卡牌类型。
    
public:
    // 初始化卡牌的名称、花费、类型和描述。
    Card(std::string name, int cost, CardType type)
        : name(name), cost(cost), type(type) {}
    // 确保派生类的析构函数被正确调用，避免资源泄漏。
    virtual ~Card() = default;
    


    

     // 访问器
    std::string getName() const { return name; } // 获取卡牌名称
    int getCost() const { return cost; } // 获取卡牌花费。
    CardType getType() const { return type; } // 获取卡牌类型
    // 虚方法 
    virtual std::string getDescription() const = 0; // 添加获取卡片说明的虚方法


    // 默认情况下，卡牌的胜利点数为0
    // 胜利点数卡会覆盖此方法，返回相应的点数。
    virtual int getVictoryPoints() const { return 0; } 


    // 纯虚方法，定义卡牌被使用时的行为
    // 每个具体的卡牌类需要实现此方法，定义其具体效果。
    virtual void play(Player& player, Game& game) = 0; 
    
    
    virtual std::string toString() const { return name; }
   

    
    // 序列化支持
    virtual void serialize(std::ofstream& out) const;
    virtual void deserialize(std::ifstream& in);
    
    // JSON序列化方法
    virtual json toJson() const {
        json j;
        j["name"] = name;
        j["cost"] = cost;
        j["type"] = static_cast<int>(type);
        return j;
    }



    // 静态工厂方法
    // 根据卡牌名称创建相应的卡牌对象。
    static std::shared_ptr<Card> createCard(const std::string& cardName); 
    // json 静态工厂方法
    static std::shared_ptr<Card> fromJson(const json& j);
}; 