#pragma once
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
        game.logAction(player.getName(), "Village utilisé (+1 carte, +2 actions)"); // 使用了村庄 (+1卡片, +2行动)
    }
    
    std::string getDescription() const override {
        return "Village (3 pièces)\nEffet : +1 carte, +2 actions\nDescription : Carte de base pour générer des chaînes d'actions.";
        // "村庄 (3金币)\n效果: +1卡片, +2行动\n说明: 基础的行动链生成卡";
    }
};

// 2. 木工 (Woodcutter/ Bûcheron )
class Woodcutter : public Card {
public:
    Woodcutter() : Card("Woodcutter", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.addBuy(1);
        player.addCoin(2);
        game.logAction(player.getName(), "Charpentier utilisé (+1 achat, +2 pièces)"); // 使用了木工 (+1购买, +2金币)
    }
    
    std::string getDescription() const override {
        return "Charpentier (3 pièces)\nEffet : +1 achat, +2 pièces\nDescription : Offre des achats supplémentaires et des pièces.";//"木工 (3金币)\n效果: +1购买, +2金币\n说明: 提供额外的购买机会和金币";
    }
};

// 3. 民兵 (Militia / Milice)
class Militia : public Card {
public:
    Militia() : Card("Militia", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.addCoin(2);
        game.logAction(player.getName(), "Milicien utilisé (+2 pièces, les autres joueurs doivent défausser jusqu'à 3 cartes"); // 使用了民兵 (+2金币，其他玩家需弃牌至3张)
        game.otherPlayersDiscardToThree(player);
    }
    
    std::string getDescription() const override {
        return "Milicien (4 pièces)\nEffet : +2 pièces, les autres joueurs doivent défausser jusqu'à 3 cartes\nDescription : Carte d'attaque qui force les autres joueurs à défausser des cartes."; 
        // 民兵 (4金币)\n效果: +2金币, 其他玩家弃牌至3张\n说明: 攻击卡，迫使其他玩家弃牌
    }
};

// 4. 市场 (Market / Marché)
class Market : public Card {
public:
    Market() : Card("Market", 5, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(1);
        player.addAction(1);
        player.addBuy(1);
        player.addCoin(1);
        game.logAction(player.getName(), "Marché utilisé (+1 carte, +1 action, +1 achat, +1 pièce)"); // 使用了市场 (+1卡片, +1行动, +1购买, +1金币)
    }
    
    std::string getDescription() const override {
        return "Marché (5 pièces)\nEffet : +1 carte, +1 action, +1 achat, +1 pièce\nDescription : Amélioration globale pour toutes les capacités."; // 市场 (5金币)\n效果: +1卡片, +1行动, +1购买, +1金币\n说明: 全面提升卡
    }
};

// 5. 铁匠 (Smithy / Forgeron)
class Smithy : public Card {
public:
    Smithy() : Card("Smithy", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(3);
        game.logAction(player.getName(), "Forgeron utilisé (+3 cartes)"); // 使用了铁匠 (+3卡片)
    }
    
    std::string getDescription() const override {
        return "Forgeron (4 pièces)\nEffet : +3 cartes\nDescription : Carte puissante pour piocher davantage de cartes.";  // 铁匠 (4金币)\n效果: +3卡片\n说明: 强力的抽牌卡
    }
};

// 6. 议会厅 (Council Room / Salle du Conseil)
class CouncilRoom : public Card {
public:
    CouncilRoom() : Card("Council Room", 5, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(4);
        player.addBuy(1);
        game.otherPlayersDraw(player, 1);
        game.logAction(player.getName(), "Salle du Conseil utilisée (+4 cartes, +1 achat, chaque autre joueur pioche 1 carte)"); // 使用了议会厅 (+4卡片, +1购买, 其他玩家各抽1张)
    }
    
    std::string getDescription() const override {
        return "Salle du Conseil (5 pièces)\nEffet : +4 cartes, +1 achat, chaque autre joueur pioche 1 carte\nDescription : Carte puissante pour piocher, mais qui permet aussi aux autres joueurs de piocher une carte."; // 议会厅 (5金币)\n效果: +4卡片, +1购买, 其他玩家各抽1张\n说明: 强力的抽牌卡，但会让其他玩家也抽牌
    }
};

// 7. 钱庄 (Moneylender / Prêteur sur gages)
class Moneylender : public Card {
public:
    Moneylender() : Card("Moneylender", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        if (player.trashCopperFromHand()) {
            player.addCoin(3);
            game.logAction(player.getName(), "Trésorerie utilisée (abandonnant un Cuivre, +3 pièces)"); // 使用了钱庄 (废弃一张铜币, +3金币)
        } else {
            game.logAction(player.getName(), "Trésorerie utilisée, mais pas de Cuivre pour abandonnant"); // 使用了钱庄，但没有铜币可废弃
        }
    }
    
    std::string getDescription() const override {
        return "Trésorerie (4 pièces)\nEffet : Abandonnez un Cuivre pour obtenir +3 pièces\nDescription : Carte permettant de transformer un Cuivre en plus de pièces."; // 钱庄 (4金币)\n效果: 废弃一张铜币，获得3金币\n说明: 将铜币转化为更多金币的卡片
    }
};

// 8. 护城河 (Moat / Douves)
class Moat : public Card {
public:
    Moat() : Card("Moat", 2, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(2);
        game.logAction(player.getName(), "Douves utilisé (+2 cartes)");  // 使用了护城河 (+2卡片)
    }
    
    std::string getDescription() const override {
        return "Douves (2 pièces)\nEffet : +2 cartes, peut bloquer les attaques\nDescription : Carte pratique qui permet à la fois de piocher et de se défendre."; // 护城河 (2金币)\n效果: +2卡片，可以防御攻击\n说明: 既能抽牌又能防御的实用卡片
    }
};

// 9. 工场 (Workshop / Atelier)
class Workshop : public Card {
public:
    Workshop() : Card("Workshop", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        game.gainCardUpToCost(player, 4);
        game.logAction(player.getName(), "Atelier utilisé (obtenez une carte coûtant jusqu’à 4 pièces)");  // 使用了工场 (获得一张不超过4块的卡片)
    }
    
    std::string getDescription() const override {
        return "Atelier (3 pièces)\nEffet : Obtenez une carte coûtant jusqu’à 4 pièces\nDescription : Utile pour acquérir des cartes de faible ou moyen coût."; // 工场 (3金币)\n效果: 获得一张不超过4金币的卡片\n说明: 帮助获得中低费用卡片
    }
};

// 10. 地窖 (Cellar / Cave)
class Cellar : public Card {
public:
    Cellar() : Card("Cellar", 2, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.addAction(1);
        int discarded = player.discardAndDraw();
        game.logAction(player.getName(), 
            "Cave utilisée (+1 action, défaussez" + std::to_string(discarded) + "cartes et piochez à nouveau)"); // 使用了地窖 (+1行动, 弃掉" + std::to_string(discarded) + "张牌并重抽)
    }
    
    std::string getDescription() const override {
        return "Cave (2 pièces)\nEffet : +1 action, vous pouvez défausser n’importe quel nombre de cartes, puis en piocher autant\nDescription : Carte pratique pour optimiser votre main."; // 地窖 (2金币)\n效果: +1行动，可以弃任意张牌，然后抽相同数量的牌\n说明: 帮助优化手牌的实用卡片
    }
};