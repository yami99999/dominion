#pragma once
#include <vector> // 使用动态数组（std::vector）来管理玩家和供应堆中的卡牌。
#include <map> // 使用映射（std::map）来关联卡牌名称与对应的卡牌堆。
#include <memory> // 使用智能指针（std::shared_ptr）管理卡牌对象的生命周期。
#include <string> // 使用字符串（std::string）处理玩家名称和卡牌名称
#include <fstream> // 处理文件输入输出，用于保存和加载游戏状态。
#include <iomanip> // 用于格式化输出，例如设置字段宽度。
#include "Player.h" // 引入 Player 类的定义。
#include "Utils.h" // 引入辅助工具类 Utils。
#include "GameLogger.h" // 引入游戏日志类 GameLogger。

class Card;  // 前向声明: 避免在此头文件中引入 Card 类的完整定义，从而减少编译依赖。

class Game {
private:
    std::vector<Player> players; // 存储游戏中的所有玩家。
    std::map<std::string, std::vector<std::shared_ptr<Card>>> supply; // 存储供应堆，每种卡牌名称对应一个卡牌堆（动态数组）。
    int currentPlayer; // 当前回合的玩家索引。
    int turnCount; // 当前回合数。
    GameLogger logger; // 游戏日志对象，用于记录游戏过程中的事件
    
    void displayResults(const std::vector<std::pair<std::string, int>>& scores); // 显示游戏结束后的得分结果。
    void displayStats(); // 显示游戏统计信息。
    void displayFinalState() const; //  显示游戏结束时的最终状态。
    
public:
    // 构造函数
    explicit Game(int numPlayers = 0); // 避免隐式转换导致的歧义: 参数 numPlayers 默认为 0，表示创建一个临时的游戏实例（用于加载存档）。
    
    // 核心函数
    void initialize(); // 初始化游戏，包括设置供应堆和玩家的起始牌组。
    void playGame(); // 启动游戏主循环，直到游戏结束
    void playTurn(); //  处理单个玩家的回合。
    bool isGameOver() const; // 检查游戏是否结束。(检查不改变状态)
    
    // 阶段方法
    void playActionPhase(); // 行动阶段的处理逻辑。
    void playBuyPhase();  // 购买阶段的处理逻辑。
    void playCleanupPhase(); // 清理阶段的处理逻辑。
    
    bool isProvinceEmpty() const; // 检查“行省”堆是否已空 (检查不改变状态)
    int getEmptyPiles() const;  // 获取当前空的供应堆数量。(检查不改变状态)
    /* 计算所有玩家的最终得分。(不改变状态) */
    // 返回容器,其中为{玩家名,分数}:  {{"Alice", 50}, {"Bob", 40}, {"Charlie", 45}}
    std::vector<std::pair<std::string, int>> calculateFinalScores() const; 
    

    // 存档方法
    void saveGame(const std::string& filename); // 存当前游戏状态到指定文件。
    void loadGame(const std::string& filename); // 从指定文件加载游戏状态。

    // 日志方法
    void logAction(const std::string& playerName, const std::string& action);  // 记录玩家的动作。
    

    // 其他玩家交互方法
    void otherPlayersDiscardToThree(const Player& currentPlayer); // 使除当前玩家外的其他玩家弃牌至三张。
    void otherPlayersDraw(const Player& currentPlayer, int count); // 使除当前玩家外的其他玩家抽取指定数量的牌。
    void gainCardUpToCost(Player& player, int maxCost); // 允许玩家获得一张不超过指定花费的卡牌。
    

    // 显示方法
    void showSupplyCardDescription(const std::string& cardName) const;  // 显示指定卡牌的描述。
    void displaySupplyPiles() const; // 显示所有供应堆的状态
    


    // 获取当前玩家状态的方法

    // 当前玩家名字
    std::string getCurrentPlayerName() const { 
        return players[currentPlayer].getName();
    }
    // 当前玩家动作
    int getCurrentPlayerActions() const {
        return players[currentPlayer].getActions();
    }
    // 当前玩家购买
    int getCurrentPlayerBuys() const {
        return players[currentPlayer].getBuys();
    }
    // 当前玩家金钱
    int getCurrentPlayerCoins() const {
        return players[currentPlayer].getCoins();
    }
}; 