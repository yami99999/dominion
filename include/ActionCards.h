#pragma once
#include <iostream>
#include <limits>
#include <algorithm>
#include <vector>
#include "Card.h"
#include "Player.h"
#include "Game.h"

// 1. 村庄 (Village / Village )
class Village : public Card {
public:
    Village() : Card("Village", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(1);
        player.addAction(2);
        game.logAction(player.getName(), "Village utilisé (+1 carte, +2 action)");
    }
    
    std::string getDescription() const override {
        return "Village (3 pièces)\nEffet : carte +1, action +2\nDescription : carte de génération de chaîne d'action de base";
    }
};

// 2. 木工 (Woodcutter / Bûcheron )
class Woodcutter : public Card {
public:
    Woodcutter() : Card("Woodcutter", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.addBuy(1);
        player.addCoin(2);
        game.logAction(player.getName(), "Bûcheron utilisé(+1 achat, +2 pièces)");
    }
    
    std::string getDescription() const override {
        return "Bûcheron (3 pièces)\nEffet : +1 achat, +2 pièces\nDescription : Offre des achats supplémentaires et des pièces.";
    }
};

// 3. 民兵 (Militia / Milice)
class Militia : public Card {
public:
    Militia() : Card("Militia", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.addCoin(2);
        game.logAction(player.getName(), "Milice utilisé (+2 pièces, les autres joueurs doivent défausser jusqu'à 3 cartes)");
        game.otherPlayersDiscardToThree(player);
    }
    
    std::string getDescription() const override {
        return "Milice  (4 pièces)\nEffet : +2 pièces, les autres joueurs doivent défausser jusqu'à 3 cartes\nDescription : Carte d'attaque qui force les autres joueurs à défausser des cartes.";
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
        game.logAction(player.getName(), "Marché utilisé (+1 carte, +1 action, +1 achat, +1 pièce)");
    }
    
    std::string getDescription() const override {
        return "Marché (5 pièces)\nEffet : +1 carte, +1 action, +1 achat, +1 pièce\nDescription : Amélioration globale pour toutes les capacités.";
    }
};

// 5. 铁匠 (Smithy / Forgeron)
class Smithy : public Card {
public:
    Smithy() : Card("Smithy", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(3);
        game.logAction(player.getName(), "Forgeron utilisé (+3 cartes)");
    }
    
    std::string getDescription() const override {
        return "Forgeron (4 pièces)\nEffet : +3 cartes\nDescription : Carte puissante pour piocher davantage de cartes.";
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
        game.logAction(player.getName(), "alle du Conseil utilisée (+4 cartes, +1 achat, chaque autre joueur pioche 1 carte)");
    }
    
    std::string getDescription() const override {
        return "Salle du Conseil (5 pièces)\nEffet : +4 cartes, +1 achat, chaque autre joueur pioche 1 carte\nDescription : Carte puissante pour piocher, mais qui permet aussi aux autres joueurs de piocher une carte.";
    }
};

// 7. 钱庄 (Moneylender / Trésorerie )
class Moneylender : public Card {
public:
    Moneylender() : Card("Moneylender", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        if (player.trashCopperFromHand()) {
            player.addCoin(3);
            game.logAction(player.getName(), "Trésorerie utilisée (abandonnant un Cuivre, +3 pièces)");
        } else {
            game.logAction(player.getName(), "Trésorerie utilisée, mais aucun Cuivre à abandonner.");
        }
    }
    
    std::string getDescription() const override {
        return "Trésorerie (4 pièces)\nEffet : Abandonnez un Cuivre pour obtenir +3 pièces\nDescription : Carte permettant de transformer un Cuivre en plus de pièces.";
    }
};

// 8. 护城河 (Moat / Douves)
class Moat : public Card {
public:
    Moat() : Card("Moat", 2, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(2);
        game.logAction(player.getName(), "Douve utilisé (+2 cartes)");
    }
    
    std::string getDescription() const override {
        return "Douve  (2 pièces)\nEffet : +2 cartes, peut bloquer les attaques\nDescription : Carte pratique qui permet à la fois de piocher et de se défendre.";
    }
};

// 9. 工场 (Workshop / Atelier)
class Workshop : public Card {
public:
    Workshop() : Card("Workshop", 3, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        game.gainCardUpToCost(player, 4);
        game.logAction(player.getName(), "Atelier utilisé (obtenez une carte coûtant jusqu’à 4 pièces)");
    }
    
    std::string getDescription() const override {
        return "Atelier (3 pièces)\nEffet : Obtenez une carte coûtant jusqu’à 4 pièces\nDescription : Utile pour acquérir des cartes de faible ou moyen coût.";
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
            "Cave utilisée (+1 action, défaussez " + std::to_string(discarded) + "cartes et piochez à nouveau)");
    }
    
    std::string getDescription() const override {
        return "Cave (2 pièces)\nEffet : +1 action, vous pouvez défausser n’importe quel nombre de cartes, puis en piocher autant\nDescription : Carte pratique pour optimiser votre main.";
    }
};

// 11. 女巫 (Witch / Sorcière ): +2卡片，其他玩家各获得一张诅咒卡
class Witch : public Card {
public:
    Witch() : Card("Witch", 5, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(2);
        game.otherPlayersGainCurse(player);
        game.logAction(player.getName(), "Sorcière utilisée (+2 cartes, chaque autre joueur reçoit une carte Malédiction)");
    }
    
    std::string getDescription() const override {
        return "Sorcière (5 pièces)\nEffet : +2 cartes, chaque autre joueur reçoit une carte Malédiction\nDescription : Carte d'attaque puissante qui inflige des points négatifs aux adversaires.";
    }
};

// 12. 盗贼 (Thief / Voleur): 查看其他玩家牌库顶的2张牌，可以废除其中的财宝牌并获得之
class Thief : public Card {
public:
    Thief() : Card("Thief", 4, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        game.resolveThiefEffect(player);
        game.logAction(player.getName(), "Voleur utilisé (regardez et potentiellement gagnez des cartes Trésor des autres joueurs)");
    }
    
    std::string getDescription() const override {
        return "Voleur (4 pièces)\nEffet : Regardez les 2 premières cartes des pioches des autres joueurs, éliminez leurs cartes Trésor et gagnez-les\nDescription : Carte d'attaque permettant de voler les trésors des adversaires.";
    }
};

// 13. 花园 (Gardens / Jardin ): 每10张牌价值1分
class Gardens : public Card {
public:
    Gardens() : Card("Gardens", 4, CardType::VICTORY) {}
    
    void play(Player& player, Game& game) override {} // 胜利点数卡不需要实现play
    
    int getVictoryPoints() const override {
        // 实际计分在Game类中进行，因为需要知道玩家的总牌数
        return 0; // 这里返回0，实际分数在计算总分时处理
    }
    
    std::string getDescription() const override {
        return "Jardin (4 pièces)\nEffet : 1 point de victoire pour chaque 10 cartes que vous possédez\nDescription : Carte de victoire spéciale qui récompense une grande quantité de cartes.";
    }
};

// 14. 教堂 (Chapel / Chapelle): 可以废除手牌中最多4张牌
class Chapel : public Card {
public:
    Chapel() : Card("Chapel", 2, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        game.resolveChapelEffect(player);
        game.logAction(player.getName(), "Chapelle utilisée (jusqu’à 4 cartes de la main supprimées)");
    }
    
    std::string getDescription() const override {
        return "Chapelle (2 pièces)\nEffet : Supprimez jusqu’à 4 cartes de votre main\nDescription : Carte pratique pour optimiser le contenu de votre deck.";
    }
};

// 15. 盛宴 (Feast / Festin ): 废除此牌，获得一张不超过5块的卡片
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
            game.logAction(player.getName(), "Festin utilisé (cette carte est supprimée, obtenez une carte coûtant jusqu’à 5 pièces)");
        }
    }
    
    std::string getDescription() const override {
        return "Festin (4 pièces)\nEffet : Supprimez cette carte et obtenez une carte coûtant jusqu’à 5 pièces\nDescription : Bonne option pour améliorer les cartes de votre deck.";
    }
};

// 16. 实验室 (Laboratory): +2卡片，+1行动
class Laboratory : public Card {
public:
    Laboratory() : Card("Laboratory", 5, CardType::ACTION) {}
    
    void play(Player& player, Game& game) override {
        player.drawCard(2);
        player.addAction(1);
        game.logAction(player.getName(), "Laboratoire utilisé (+2 cartes, +1 action)");
    }
    
    std::string getDescription() const override {
        return "Laboratoire (5 pièces)\nEffet : +2 cartes, +1 action\nDescription : Carte puissante pour piocher et générer des actions supplémentaires.";
    }
};