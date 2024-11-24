#pragma once
#include <iostream>
#include <limits>
#include <algorithm>
#include <vector>
#include "Card.h"
#include "Player.h"
#include "Game.h"

// 1. 村庄 (Village)
class Village : public Card {
public:
    Village() : Card("Village", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(1);
        player.addAction(2);
        game.logAction(player.getName(), "使用了村庄 (+1卡片, +2行动)");
    }
    
    std::string getDescription() const override {
        return "村庄 (3金币)\n效果: +1卡片, +2行动\n说明: 基础的行动链生成卡";
    }
};

// 2. 木工 (Woodcutter)
class Woodcutter : public Card {
public:
    Woodcutter() : Card("Woodcutter", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.addBuy(1);
        player.addCoin(2);
        game.logAction(player.getName(), "使用了木工 (+1购买, +2金币)");
    }
    
    std::string getDescription() const override {
        return "木工 (3金币)\n效果: +1购买, +2金币\n说明: 提供额外的购买机会和金币";
    }
};

// 3. 民兵 (Militia)
class Militia : public Card {
public:
    Militia() : Card("Militia", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.addCoin(2);
        game.logAction(player.getName(), "使用了民兵 (+2金币，其他玩家需弃牌至3张)");
        game.otherPlayersDiscardToThree(player);
    }
    
    std::string getDescription() const override {
        return "民兵 (4金币)\n效果: +2金币, 其他玩家弃牌至3张\n说明: 攻击卡，迫使其他玩家弃牌";
    }
};

// 4. 市场 (Market)
class Market : public Card {
public:
    Market() : Card("Market", 5, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(1);
        player.addAction(1);
        player.addBuy(1);
        player.addCoin(1);
        game.logAction(player.getName(), "使用了市场 (+1卡片, +1行动, +1购买, +1金币)");
    }
    
    std::string getDescription() const override {
        return "市场 (5金币)\n效果: +1卡片, +1行动, +1购买, +1金币\n说明: 全面提升卡";
    }
};

// 5. 铁匠 (Smithy)
class Smithy : public Card {
public:
    Smithy() : Card("Smithy", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(3);
        game.logAction(player.getName(), "使用了铁匠 (+3卡片)");
    }
    
    std::string getDescription() const override {
        return "铁匠 (4金币)\n效果: +3卡片\n说明: 强力的抽牌卡";
    }
};

// 6. 议会厅 (Council Room)
class CouncilRoom : public Card {
public:
    CouncilRoom() : Card("Council Room", 5, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(4);
        player.addBuy(1);
        game.otherPlayersDraw(player, 1);
        game.logAction(player.getName(), "使用了议会厅 (+4卡片, +1购买, 其他玩家各抽1张)");
    }
    
    std::string getDescription() const override {
        return "议会厅 (5金币)\n效果: +4卡片, +1购买, 其他玩家各抽1张\n说明: 强力的抽牌卡，但会让其他玩家也抽牌";
    }
};

// 7. 钱庄 (Moneylender)
class Moneylender : public Card {
public:
    Moneylender() : Card("Moneylender", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        if (player.trashCopperFromHand()) {
            player.addCoin(3);
            game.logAction(player.getName(), "使用了钱庄 (废弃一张铜币, +3金币)");
        } else {
            game.logAction(player.getName(), "使用了钱庄，但没有铜币可废弃");
        }
    }
    
    std::string getDescription() const override {
        return "钱庄 (4金币)\n效果: 废弃一张铜币，获得3金币\n说明: 铜币转化为更多金币的卡片";
    }
};

// 8. 护城河 (Moat)
class Moat : public Card {
public:
    Moat() : Card("Moat", 2, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(2);
        game.logAction(player.getName(), "使用了护城河 (+2卡片)");
    }
    
    std::string getDescription() const override {
        return "护城河 (2金币)\n效果: +2卡片，可以防御攻击\n说明: 既能抽牌又能防御的实用卡片";
    }
};

// 9. 工场 (Workshop)
class Workshop : public Card {
public:
    Workshop() : Card("Workshop", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        game.gainCardUpToCost(player, 4);
        game.logAction(player.getName(), "使用了工场 (获得一张不超过4块的卡片)");
    }
    
    std::string getDescription() const override {
        return "工场 (3金币)\n效果: 获得一张不超过4金币的卡片\n说明: 帮助获得中低费用卡片";
    }
};

// 10. 地窖 (Cellar)
class Cellar : public Card {
public:
    Cellar() : Card("Cellar", 2, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.addAction(1);
        int discarded = player.discardAndDraw();
        game.logAction(player.getName(), 
            "使用了地窖 (+1行动, 弃掉" + std::to_string(discarded) + "张牌并重抽)");
    }
    
    std::string getDescription() const override {
        return "地窖 (2金币)\n效果: +1行动，可以弃任意张牌，然后抽相同数量的牌\n说明: 帮助优化手牌的实用卡片";
    }
};

// 1. 女巫 (Witch): +2卡片，其他玩家各获得一张诅咒卡
class Witch : public Card {
public:
    Witch() : Card("Witch", 5, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(2);
        game.otherPlayersGainCurse(player);
        game.logAction(player.getName(), "使用了女巫 (+2卡片，其他玩家各获得一张诅咒卡)");
    }
    
    std::string getDescription() const override {
        return "女巫 (5金币)\n效果: +2卡片，其他玩家各获得一张诅咒卡\n说明: 强力的攻击卡，可以给对手负分";
    }
};

// 2. 盗贼 (Thief): 查看其他玩家牌库顶的2张牌，可以废除其中的财宝牌并获得之
class Thief : public Card {
public:
    Thief() : Card("Thief", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        game.resolveThiefEffect(player);
        game.logAction(player.getName(), "使用了盗贼 (查看并可能获得其他玩家的财宝牌)");
    }
    
    std::string getDescription() const override {
        return "盗贼 (4金币)\n效果: 查看其他玩家牌库顶的2张牌，可以废除其中的财宝牌并获得之\n说明: 攻击卡，可以偷取对手的财宝";
    }
};

// 3. 花园 (Gardens): 每10张牌价值1分
class Gardens : public Card {
public:
    Gardens() : Card("Gardens", 4, CardType::VICTORY) {}
    
    void play(Player& player, Game& game) override {} // 胜利点数卡不需要实现play
    
    int getVictoryPoints() const override {
        // 实际计分在Game类中进行，因为需要知道玩家的总牌数
        return 0; // 这里返回0，实际分数在计算总分时处理
    }
    
    std::string getDescription() const override {
        return "花园 (4金币)\n效果: 每拥有10张牌价值1分\n说明: 特殊的胜利点数卡，鼓励收集更多的牌";
    }
};

// 4. 教堂 (Chapel): 可以废除手牌中最多4张牌
class Chapel : public Card {
public:
    Chapel() : Card("Chapel", 2, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        game.resolveChapelEffect(player);
        game.logAction(player.getName(), "使用了教堂 (可以废除手牌中最多4张牌)");
    }
    
    std::string getDescription() const override {
        return "教堂 (2金币)\n效果: 可以废除手牌中最多4张牌\n说明: 帮助优化牌组的实用卡片";
    }
};

// 5. 盛宴 (Feast): 废除此牌，获得一张不超过5块的卡片
class Feast : public Card {
public:
    Feast() : Card("Feast", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        // 找到这张卡在手牌中的位置
        auto& playerHand = player.getHand();
        auto it = std::find_if(playerHand.begin(), playerHand.end(),
            [this](const auto& card) { return card->getName() == "Feast"; });
        
        if (it != playerHand.end()) {
            size_t index = std::distance(playerHand.begin(), it);
            player.trashCardFromHand(index);
            game.gainCardUpToCost(player, 5);
            game.logAction(player.getName(), "使用了盛宴 (废除此牌，获得一张不超过5块的卡片)");
        }
    }
    
    std::string getDescription() const override {
        return "盛宴 (4金币)\n效果: 废除此牌，获得一张不超过5块的卡片\n说明: 一次性升级卡片的好选择";
    }
};

// 6. 实验室 (Laboratory): +2卡片，+1行动
class Laboratory : public Card {
public:
    Laboratory() : Card("Laboratory", 5, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(2);
        player.addAction(1);
        game.logAction(player.getName(), "使用了实验室 (+2卡片, +1行动)");
    }
    
    std::string getDescription() const override {
        return "实验室 (5金币)\n效果: +2卡片, +1行动\n说明: 强力的抽牌和行动生成卡";
    }
};