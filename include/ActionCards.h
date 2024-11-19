#pragma once
#include <iostream>     // 为 std::cout, std::cin
#include <limits>      // 为 std::numeric_limits
#include <algorithm>   // 为 std::sort
#include <vector>      // 为 std::vector
#include "Card.h"
#include "Player.h"
#include "Game.h"

// 基础显示手牌的辅助函数
inline void showCurrentHand(Player& player, const std::string& cardName) {
    std::cout << "\n[使用 " << cardName << " 之前的手牌]\n";
    player.showHand();
    std::cout << std::string(40, '-') << "\n";
}

// 1. 村庄 (Village): +1卡片，+2行动
class Village : public Card {
public:
    Village() : Card("Village", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        showCurrentHand(player, "Village");
        player.drawCard(1);
        player.addAction(2);
        game.logAction(player.getName(), "使用了村庄 (+1卡片, +2行动)");
    }
};

// 2. 木工 (Woodcutter): +1购买，+2金币
class Woodcutter : public Card {
public:
    Woodcutter() : Card("Woodcutter", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        showCurrentHand(player, "Woodcutter");
        player.addBuy(1);
        player.addCoin(2);
        game.logAction(player.getName(), "使用了木工 (+1购买, +2金币)");
    }
};

// 3. 民兵 (Militia): +2金币，其他玩家弃牌至3张
class Militia : public Card {
public:
    Militia() : Card("Militia", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        showCurrentHand(player, "Militia");
        player.addCoin(2);
        game.logAction(player.getName(), "使用了民兵 (+2金币，其他玩家需弃牌至3张)");
        game.otherPlayersDiscardToThree(player);
    }
};

// 4. 市场 (Market): +1卡片，+1行动，+1购买，+1金币
class Market : public Card {
public:
    Market() : Card("Market", 5, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        showCurrentHand(player, "Market");
        player.drawCard(1);
        player.addAction(1);
        player.addBuy(1);
        player.addCoin(1);
        game.logAction(player.getName(), "使用了市场 (+1卡片, +1行动, +1购买, +1金币)");
    }
};

// 5. 铁匠 (Smithy): +3卡片
class Smithy : public Card {
public:
    Smithy() : Card("Smithy", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        showCurrentHand(player, "Smithy");
        player.drawCard(3);
        game.logAction(player.getName(), "使用了铁匠 (+3卡片)");
    }
};

// 6. 议会厅 (Council Room): +4卡片，+1购买，其他玩家各抽1张
class CouncilRoom : public Card {
public:
    CouncilRoom() : Card("Council Room", 5, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        showCurrentHand(player, "Council Room");
        player.drawCard(4);
        player.addBuy(1);
        game.otherPlayersDraw(player, 1);
        game.logAction(player.getName(), "使用了议会厅 (+4卡片, +1购买, 其他玩家各抽1张)");
    }
};

// 7. 钱庄 (Moneylender): 废弃一张铜币获得3金币
class Moneylender : public Card {
public:
    Moneylender() : Card("Moneylender", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        showCurrentHand(player, "Moneylender");
        if (player.trashCopperFromHand()) {
            player.addCoin(3);
            game.logAction(player.getName(), "使用了钱庄 (废弃一张铜币, +3金币)");
        } else {
            game.logAction(player.getName(), "使用了钱庄，但没有铜币可废弃");
        }
    }
};

// 8. 护城河 (Moat): +2卡片，可以防御攻击
class Moat : public Card {
public:
    Moat() : Card("Moat", 2, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        showCurrentHand(player, "Moat");
        player.drawCard(2);
        game.logAction(player.getName(), "使用了护城河 (+2卡片)");
    }
};

// 9. 工场 (Workshop): 获得一张不超过4块的卡片
class Workshop : public Card {
public:
    Workshop() : Card("Workshop", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        showCurrentHand(player, "Workshop");
        game.gainCardUpToCost(player, 4);
        game.logAction(player.getName(), "使用了工场 (获得一张不超过4块的卡片)");
    }
};

// 10. 地窖 (Cellar): +1行动，可以弃任意张牌，然后抽相同数量的牌
class Cellar : public Card {
public:
    Cellar() : Card("Cellar", 2, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        showCurrentHand(player, "Cellar");
        player.addAction(1);
        
        // 显示编号的手牌列表（不包括刚使用的Cellar）
        std::cout << "\n当前手牌 (输入对应数字选择要弃置的牌):\n";
        std::vector<std::shared_ptr<Card>> currentHand = player.getHand();
        bool hasCellarShown = false;  // 用于确保只显示一次提示
        
        for (size_t i = 0; i < currentHand.size(); i++) {
            if (currentHand[i]->getName() == "Cellar" && !hasCellarShown) {
                std::cout << "* " << currentHand[i]->getName() << " (已使用)\n";
                hasCellarShown = true;
            } else if (currentHand[i]->getName() != "Cellar") {
                std::cout << (i + 1) << ". " << currentHand[i]->getName() << "\n";
            }
        }
        
        std::cout << "\n请选择要弃置的卡牌编号 (用空格分隔，输入0结束):\n";
        std::cout << "注意: 弃置的牌会进入弃牌堆，并从牌库中抽取相同数量的新卡\n";
        
        std::vector<int> choices;
        int choice;
        while (std::cin >> choice && choice != 0) {
            if (choice > 0 && choice <= currentHand.size()) {
                auto card = currentHand[choice - 1];
                if (card->getName() != "Cellar") {
                    if (std::find(choices.begin(), choices.end(), choice - 1) == choices.end()) {
                        choices.push_back(choice - 1);
                        std::cout << "已选择: " << card->getName() << "\n";
                    } else {
                        std::cout << "该卡牌已经被选择\n";
                    }
                } else {
                    std::cout << "不能选择刚使用的Cellar\n";
                }
            } else {
                std::cout << "无效的选择，请重新输入\n";
            }
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        // 从大到小排序，以便从后往前删除
        std::sort(choices.rbegin(), choices.rend());
        
        // 记录弃掉的牌数和名称
        int discardCount = 0;
        std::vector<std::string> discardedCards;
        
        // 弃掉选中的牌
        for (int idx : choices) {
            if (idx >= 0 && idx < currentHand.size()) {
                discardedCards.push_back(currentHand[idx]->getName());
                player.discardCard(idx);
                discardCount++;
            }
        }
        
        // Cellar本身也进入弃牌堆
        player.discardCellar();
        
        // 抽相同数量的牌
        if (discardCount > 0) {
            std::string discardedList = "";
            for (size_t i = 0; i < discardedCards.size(); i++) {
                if (i > 0) discardedList += ", ";
                discardedList += discardedCards[i];
            }
            
            std::cout << "\n弃置了 " << discardCount << " 张牌: " << discardedList << "\n";
            player.drawCard(discardCount);
            game.logAction(player.getName(), 
                "使用了地窖 (+1行动, 弃置了 " + discardedList + ", 并抽取了 " + 
                std::to_string(discardCount) + " 张新卡)");
        } else {
            game.logAction(player.getName(), "使用了地窖 (+1行动, 没有弃置任何卡牌)");
        }
        
        // 显示更新后的手牌和牌堆状态
        std::cout << "\n更新后的状态:\n";
        player.showDeckStatus();
        player.showHand();
    }
};