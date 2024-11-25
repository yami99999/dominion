#include "Player.h"
#include "Game.h"
#include "AllCards.h"
#include "BasicCards.h"
#include "ActionCards.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>

// Constructeur : Initialiser le nom du joueur, les points d'action, les points d'achat, les pièces d'or
//  构造函数: 初始化玩家名称、行动点、购买点、金币
Player::Player(const std::string &playerName) : name(playerName), actions(1), buys(1), coins(0)
{
}

// Fonction : piocher des cartes
//  作用: 抽牌
void Player::drawCard(int count)
{
    for (int i = 0; i < count; i++)
    {
        // Si la pioche est vide, mélangez la pile de défausse et ajoutez-la à la pioche
        //  如果牌堆为空，则将弃牌堆洗牌后加入牌堆
        if (deck.empty())
        {
            // Si la pile de défausse est également vide, aucune carte ne peut être piochée
            //  如果弃牌堆也为空，则无法抽牌
            if (discard.empty())
                return;
            // Mélangez la pile de défausse et ajoutez-la au paquet
            //  弃牌堆洗牌后加入牌堆
            shuffleDeck();
        }
        // Piochez une carte du paquet
        //  从牌堆中抽取一张牌
        hand.push_back(deck.back());
        // Retirer la carte du jeu
        //  从牌堆中移除该牌
        deck.pop_back();
    }
}

// Fonction : Mélanger
//  作用: 洗牌
void Player::shuffleDeck()
{
    deck.insert(deck.end(), discard.begin(), discard.end());
    discard.clear();

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
}

// Fonction : défausser les cartes
//  作用: 弃牌
void Player::discardHand()
{
    // Ajouter des cartes en main à la pile de défausse
    //  将手牌加入弃牌堆
    discard.insert(discard.end(), hand.begin(), hand.end());
    // Main claire
    //  清空手牌
    hand.clear();
}

// Fonction : Initialiser le deck du joueur
//  作用: 初始化玩家牌组
void Player::initializeDeck()
{
    try
    {
        // Deck initial : 7 pièces de cuivre et 3 manoirs
        //  初始牌组：7张铜币和3张庄园
        for (int i = 0; i < 7; i++)
        {
            deck.push_back(Card::createCard("Copper"));
        }
        for (int i = 0; i < 3; i++)
        {
            deck.push_back(Card::createCard("Estate"));
        }
        // mélanger
        //  洗牌
        shuffleDeck();
        // Dessine la main de départ
        //  抽取起始手牌
        drawCard(5);
    }
    // Échec de l'initialisation du deck du joueur, exception interceptée
    //  初始化玩家牌组失败, 捕获异常
    catch (const std::exception &e)
    {
        std::cerr << "Échec de l'initialisation du deck : " << e.what() << std::endl;
        throw;
    }
}

// Fonction : Afficher l'état du deck du joueur
//  作用: 显示玩家牌堆状态
void Player::showDeckStatus() const
{
    std::cout << "Deck: " << deck.size() << " cartes, "
              << "Main: " << hand.size() << " cartes, "
              << "Défausse: " << discard.size() << " cartes\n";
}

// Fonction : Afficher les cartes de la main du joueur
//  作用: 显示玩家手牌
void Player::showHand() const
{
    std::cout << "Main: ";
    for (const auto &card : hand)
    {
        std::cout << card->getName() << " ";
    }
    std::cout << "\n";
}

// Fonction : Vérifier si le joueur possède une carte action
//  作用: 检查玩家是否有行动卡
bool Player::hasActionCard() const
{
    return std::any_of(hand.begin(), hand.end(),
                       [](const auto &card)
                       { return card->getType() == CardType::ACTION; });
}

// Fonction : Exécuter la carte d'action
// Vérifiez s'il y a suffisamment de points d'action
// Rechercher des cartes
// utilise la carte
// Ajouter les cartes utilisées à la pile de défausse
// Retirez cette carte de la main
//  作用: 执行行动卡
//  检查是否有足够的行动点
//  查找卡片
//  使用卡片
//  将使用过的卡片加入弃牌堆
//  从手牌中移除该卡片
bool Player::playAction(const std::string &cardName, Game &game)
{
    // Vérifiez s'il y a suffisamment de points d'action
    //  检查是否有足够的行动点
    if (actions <= 0)
    {
        std::cout << "Pas assez de points d'action\n";
        return false;
    }

    // Rechercher des cartes
    // itérateur -pointe vers l'élément trouvé
    // std::find_if partira de hand.begin() et appellera tour à tour l'expression Lambda sur chaque carte :
    // Appelez card->getName() pour obtenir le nom et effectuez une comparaison indépendante de la casse avec le nom cible cardName.
    // Si Utils::equalIgnoreCase renvoie true, cela signifie qu'un élément répondant aux conditions est trouvé et la recherche s'arrête.
    // Si aucun élément correspondant n'est trouvé, renvoie hand.end().
    //  查找卡片
    // 迭代器 - 指向查找到的元素
    // std::find_if 会从 hand.begin() 开始，依次对每个 card 调用 Lambda 表达式：
    // 调用 card->getName() 获取名称，与目标名称 cardName 进行大小写无关的比较。
    // 如果 Utils::equalIgnoreCase 返回 true，表示找到了符合条件的元素，停止查找。
    // 如果找不到符合条件的元素，则返回 hand.end()。
    auto it = std::find_if(hand.begin(), hand.end(),
                           [&cardName](const auto &card)
                           {
                               return Utils::equalIgnoreCase(card->getName(), cardName);
                           });
    // Si la carte est introuvable
    //  如果找不到该卡片
    if (it == hand.end())
    {
        std::cout << "La carte est introuvable\n";
        return false;
    }
    // Vérifiez s'il s'agit d'une carte action
    //  检查是否是行动卡
    if ((*it)->getType() != CardType::ACTION)
    {
        std::cout << "Ceci n'est pas une carte d'action\n";
        return false;
    }

    // utilise la carte
    //  使用卡片
    try
    {
        (*it)->play(*this, game);
        // Ajouter les cartes utilisées à la pile de défausse
        //  将使用过的卡片加入弃牌堆
        discard.push_back(*it);
        // Retirez cette carte de la main
        //  从手牌中移除该卡片
        hand.erase(it);
        return true;
    }
    // Une erreur se produit lors de l'utilisation de la carte, interceptez l'exception
    //  使用卡片时发生错误, 捕获异常
    catch (const std::exception &e)
    {
        std::cout << "Une erreur s'est produite lors de l'utilisation de la carte: " << e.what() << "\n";
        return false;
    }
}

// Fonction : Exécuter la carte de richesse
//  作用: 执行财富卡
bool Player::playTreasure(const std::string &cardName)
{
    // itérateur -pointe vers l'élément trouvé
    //  迭代器 - 指向查找到的元素
    auto it = std::find_if(hand.begin(), hand.end(),
                           [&cardName](const auto &card)
                           { return card->getName() == cardName; });
    // Si la carte est introuvable
    //  如果找不到该卡片
    if (it == hand.end())
    {
        return false;
    }
    // Vérifiez s'il s'agit d'une carte de richesse
    //  检查是否是财富卡
    if ((*it)->getType() != CardType::TREASURE)
    {
        return false;
    }

    // Créer un objet de jeu temporaire car les cartes de richesse n'utilisent pas l'état du jeu
    //  创建一个临时Game对象，因为财富卡不使用游戏状态
    static Game dummyGame(0); // Utilisez des objets statiques pour éviter les créations répétées
                              //  使用静态对象避免重复创建
    (*it)->play(*this, dummyGame);
    // Ajouter les cartes utilisées à la pile de défausse
    //  将使用过的卡片加入弃牌堆
    discard.push_back(*it);
    // Retirez cette carte de la main
    //  从手牌中移除该卡片
    hand.erase(it);
    return true;
}

// Fonction : Obtenez les cartes de richesse dans la main du joueur -utilisé pour utiliser automatiquement toutes les cartes de richesse
// Parcourez les cartes à main et ajoutez la carte de richesse au conteneur
//  作用: 获取玩家手牌中的财富卡 - 用于自动使用所有财富卡
//  遍历手牌，将财富卡加入容器
std::vector<std::shared_ptr<Card>> Player::getTreasureCards() const
{
    std::vector<std::shared_ptr<Card>> treasures; // Conteneur pour stocker les cartes de richesse
                                                  //  存储财富卡的容器
    for (const auto &card : hand)
    {
        if (card->getType() == CardType::TREASURE)
        {
            treasures.push_back(card);
        }
    }
    return treasures;
}

// Afficher automatiquement toutes les cartes de richesse en main pour faciliter l'affichage des montants numériques
//  自动展示手牌中所有财富卡, 以方便数字金额的显示
void Player::playAllTreasures()
{
    std::cout << "\nUtilisation automatique de toutes les cartes Trésor  :\n";

    auto treasures = getTreasureCards();
    if (!treasures.empty())
    {
        for (const auto &card : treasures)
        {
            // Supprimer de la main
            //  从手牌中移除
            auto it = std::find(hand.begin(), hand.end(), card);
            if (it != hand.end())
            {
                // utilise la carte
                //  使用卡片
                card->play(*this, *static_cast<Game *>(nullptr)); // Modifier ceci pour supprimer le code qui crée un nouveau jeu
                                                                  //  修改这里，移除创建新游戏的代码

                // Déplacement vers la pile de défausse
                //  移到弃牌堆
                discard.push_back(*it);
                hand.erase(it);

                std::cout << "utilisé " << card->getName() << " (+";
                if (card->getName() == "Copper")
                    std::cout << "1";
                else if (card->getName() == "Silver")
                    std::cout << "2";
                else if (card->getName() == "Gold")
                    std::cout << "3";
                std::cout << " pièces)\n";
            }
        }

        std::cout << "Total actuel des pièces : " << coins << "\n";
    }
}

// Fonction : Vérifier si le joueur possède des douves-douves-douves
//  作用: 检查玩家是否有护城河-douves-moat
bool Player::hasMoat() const
{
    // Parcourez les cartes en main et vérifiez s'il y a un fossé
    //  遍历手牌，检查是否有护城河
    for (const auto &card : hand)
    {
        if (card->getName() == "Moat")
        {
            return true;
        }
    }
    return false;
}

// Fonction : Supprimer les pièces de cuivre de la main -utilisées pour la banque
//  作用: 从手牌中移除铜币 - 用于钱庄
bool Player::trashCopperFromHand()
{
    // Itérateur -pointe vers l'élément trouvé (pièces de cuivre)
    //  迭代器 - 指向查找到的元素(copper铜币)
    auto it = std::find_if(hand.begin(), hand.end(),
                           [](const auto &card)
                           { return card->getName() == "Copper"; });

    if (it != hand.end())
    {
        trash.push_back(*it);
        hand.erase(it);
        return true;
    }
    return false;
}

// Fonction : Défaussez les cartes en trop en main jusqu'à ce que le nombre de cartes en main atteigne le nombre spécifié -utilisé pour la milice Milice -milice
//  作用: 弃掉多余的手牌, 直到手牌数量达到指定数量 - 用于民兵 Milice - militia
void Player::discardDownTo(int count)
{
    while (hand.size() > count)
    {
        std::cout << "Main actuelle: ";
        for (size_t i = 0; i < hand.size(); i++)
        {
            std::cout << i + 1 << "." << hand[i]->getName() << " ";
        }

        std::cout << "\n Veuillez sélectionner le numéro de carte à défausser : ";
        int choice;
        std::cin >> choice;

        if (choice > 0 && choice <= hand.size())
        {
            discard.push_back(hand[choice - 1]);
            hand.erase(hand.begin() + choice - 1);
        }
    }
}

// Fonction : défausser les cartes de la main et piocher des cartes -utilisé pour Cave -Cave
//  作用: 弃掉手牌并抽牌 - 用于地窖 Cave - cellar
int Player::discardAndDraw()
{
    std::cout << "Main actuelle: ";
    for (size_t i = 0; i < hand.size(); i++)
    {
        std::cout << i + 1 << "." << hand[i]->getName() << " ";
    }

    std::cout << "\nEntrez les numéros des cartes à défausser (séparés par des espaces, entrez 0 pour terminer) : ";
    int count = 0;
    std::vector<int> choices;
    int choice;

    while (std::cin >> choice && choice != 0)
    {
        if (choice > 0 && choice <= hand.size())
        {
            choices.push_back(choice - 1);
            count++;
        }
    }

    // Trier du plus grand au plus petit pour supprimer de l'arrière vers l'avant
    //  从大到小排序，以便从后往前删除
    std::sort(choices.rbegin(), choices.rend());

    for (int idx : choices)
    {
        discard.push_back(hand[idx]);
        hand.erase(hand.begin() + idx);
    }

    // Piochez le même nombre de cartes
    //  抽相同数量的牌
    drawCard(count);
    return count;
}

// Fonction : Récupère toutes les cartes du joueur
//  作用: 获取玩家所有卡片
std::vector<std::shared_ptr<Card>> Player::getAllCards() const
{
    std::vector<std::shared_ptr<Card>> allCards;
    // Combine toutes les cartes du deck
    //  合并所有牌组中的卡片
    allCards.insert(allCards.end(), deck.begin(), deck.end());
    allCards.insert(allCards.end(), hand.begin(), hand.end());
    allCards.insert(allCards.end(), discard.begin(), discard.end());
    return allCards;
}

// Fonction : Défaussez la carte avec le numéro spécifié -utilisée pour défausser les cartes
//  作用: 弃掉指定编号的卡片 - 用于弃牌
void Player::discardCard(int index)
{
    if (index >= 0 && index < hand.size())
    {
        discard.push_back(hand[index]);
        hand.erase(hand.begin() + index);
    }
}

// Fonction : mis dans la défausse après utilisation de la cave -utilisé dans la cave Cave -cave
//  作用: 使用地窖后放入弃牌堆 - 用于地窖 Cave - cellar
void Player::discardCellar()
{
    auto it = std::find_if(hand.begin(), hand.end(),
                           [](const auto &card)
                           { return card->getName() == "Cellar"; });

    if (it != hand.end())
    {
        discard.push_back(*it);
        hand.erase(it);
    }
}

// Fonction : Afficher la description de la carte -utilisé pour afficher la description de la carte
//  作用: 展示卡片描述 - 用于展示卡片描述
void Player::showCardDescription(const std::string &cardName) const
{
    for (const auto &card : hand)
    {
        if (Utils::equalIgnoreCase(card->getName(), cardName))
        {
            std::cout << "\n"
                      << card->getDescription() << "\n";
            return;
        }
    }
    std::cout << "La carte est introuvable dans la main.\n";
}

// Fonction : Mettez la carte avec le numéro spécifié dans la pile des déchets (défausse) -utilisée dans la pile des déchets (défausse/réfute)
//  作用: 将指定编号的卡片放入垃圾堆(废弃) - 用于垃圾堆(废弃/rebuts)
void Player::trashCardFromHand(size_t index)
{
    if (index < hand.size())
    {
        trash.push_back(hand[index]);
        hand.erase(hand.begin() + index);
    }
}

////Fonction : Mettez la carte avec le numéro spécifié dans la pile des déchets (défausse) -utilisée dans la pile des déchets (défausse/réfute)
// void Player::trashCardFromDeck(size_t index) {
// si (index < deck.size()) {
// trash.push_back(deck[index]);
// deck.erase(deck.begin() + index);
// }
// }
//  // 作用: 将指定编号的卡片放入垃圾堆(废弃) - 用于垃圾堆(废弃/rebuts)
//  void Player::trashCardFromDeck(size_t index) {
//      if (index < deck.size()) {
//          trash.push_back(deck[index]);
//          deck.erase(deck.begin() + index);
//      }
//  }

// Implémentation des méthodes liées à la sérialisation JSON
//  JSON 序列化相关方法实现
json Player::toJson() const
{
    json j;
    j["name"] = name;
    j["actions"] = actions;
    j["buys"] = buys;
    j["coins"] = coins;

    // Sérialiser le deck
    //  序列化牌组
    j["deck"] = json::array();
    for (const auto &card : deck)
    {
        j["deck"].push_back(card->getName());
    }

    j["hand"] = json::array();
    for (const auto &card : hand)
    {
        j["hand"].push_back(card->getName());
    }

    j["discard"] = json::array();
    for (const auto &card : discard)
    {
        j["discard"].push_back(card->getName());
    }

    j["trash"] = json::array();
    for (const auto &card : trash)
    {
        j["trash"].push_back(card->getName());
    }

    return j;
}

// Fonction : Désérialiser le statut du joueur depuis JSON -utilisé pour charger le jeu
//  作用: 从JSON反序列化玩家状态 - 用于加载游戏
void Player::fromJson(const json &j)
{
    name = j["name"].get<std::string>();
    actions = j["actions"].get<int>();
    buys = j["buys"].get<int>();
    coins = j["coins"].get<int>();

    // Efface le deck existant
    //  清空现有牌组
    deck.clear();
    hand.clear();
    discard.clear();

    // Charger le deck
    //  加载牌组
    for (const auto &cardJson : j["deck"])
    {
        deck.push_back(Card::fromJson(cardJson));
    }

    for (const auto &cardJson : j["hand"])
    {
        hand.push_back(Card::fromJson(cardJson));
    }

    for (const auto &cardJson : j["discard"])
    {
        discard.push_back(Card::fromJson(cardJson));
    }
}

// inutile
//  useless

////Fonction : Sauvegarder le statut du joueur
// void Player::saveState(std::ofstream& out) const {
// size_t nomLongueur = nom.longueur();
// out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
// out.write(name.c_str(), nameLength);
//  // 作用: 保存玩家状态
//  void Player::saveState(std::ofstream& out) const {
//      size_t nameLength = name.length();
//      out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
//      out.write(name.c_str(), nameLength);

// out.write(reinterpret_cast<const char*>(&actions), sizeof(actions));
// out.write(reinterpret_cast<const char*>(&buys), sizeof(buys));
// out.write(reinterpret_cast<const char*>(&coins), sizeof(coins));
//      out.write(reinterpret_cast<const char*>(&actions), sizeof(actions));
//      out.write(reinterpret_cast<const char*>(&buys), sizeof(buys));
//      out.write(reinterpret_cast<const char*>(&coins), sizeof(coins));

//}
// }

// void Player::loadState(std::ifstream& in) {
// size_t nomLongueur ;
// in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
// nom.resize(nameLength);
// in.read(&name[0], nameLength);
//  void Player::loadState(std::ifstream& in) {
//      size_t nameLength;
//      in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
//      name.resize(nameLength);
//      in.read(&name[0], nameLength);

// in.read(reinterpret_cast<char*>(&actions), sizeof(actions));
// in.read(reinterpret_cast<char*>(&buys), sizeof(buys));
// in.read(reinterpret_cast<char*>(&coins), sizeof(coins));
//      in.read(reinterpret_cast<char*>(&actions), sizeof(actions));
//      in.read(reinterpret_cast<char*>(&buys), sizeof(buys));
//      in.read(reinterpret_cast<char*>(&coins), sizeof(coins));

//}
// }