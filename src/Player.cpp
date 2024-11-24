#include "Player.h"
#include "Game.h"
#include "AllCards.h"
#include "BasicCards.h"
#include "ActionCards.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>

Player::Player(const std::string& playerName) : 
    name(playerName), actions(1), buys(1), coins(0) {
}

void Player::drawCard(int count) {
    for (int i = 0; i < count; i++) {
        if (deck.empty()) {
            if (discard.empty()) return;
            shuffleDeck();
        }
        hand.push_back(deck.back());
        deck.pop_back();
    }
}

void Player::shuffleDeck() {
    deck.insert(deck.end(), discard.begin(), discard.end());
    discard.clear();
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
}

void Player::discardHand() {
    discard.insert(discard.end(), hand.begin(), hand.end());
    hand.clear();
}

void Player::initializeDeck() {
    try {
        // 初始牌组：7张铜币和3张庄园
        for (int i = 0; i < 7; i++) {
            deck.push_back(Card::createCard("Copper"));
        }
        for (int i = 0; i < 3; i++) {
            deck.push_back(Card::createCard("Estate"));
        }
        shuffleDeck();
        drawCard(5); // 抽取起始手牌
    } catch (const std::exception& e) {
        std::cerr << "初始化牌组失败: " << e.what() << std::endl;
        throw;
    }
}

void Player::showDeckStatus() const {
    std::cout << "牌堆: " << deck.size() << " 张, "
              << "手牌: " << hand.size() << " 张, "
              << "弃牌: " << discard.size() << " 张\n";
}

void Player::showHand() const {
    std::cout << "手牌: ";
    for (const auto& card : hand) {
        std::cout << card->getName() << " ";
    }
    std::cout << "\n";
}

bool Player::hasActionCard() const {
    return std::any_of(hand.begin(), hand.end(),
        [](const auto& card) { return card->getType() == CardType::ACTION; });
}

bool Player::playAction(const std::string& cardName, Game& game) {
    // 检查是否有足够的行动点
    if (actions <= 0) {
        std::cout << "没有足够的行动点\n";
        return false;
    }
    
    // 查找卡片
    auto it = std::find_if(hand.begin(), hand.end(),
        [&cardName](const auto& card) {
            return Utils::equalIgnoreCase(card->getName(), cardName);
        });
    
    if (it == hand.end()) {
        std::cout << "找不到该卡片\n";
        return false;
    }
    
    // 检查是否是行动卡
    if ((*it)->getType() != CardType::ACTION) {
        std::cout << "这不是一张行动卡\n";
        return false;
    }
    
    // 使用卡片
    try {
        (*it)->play(*this, game);
        discard.push_back(*it);
        hand.erase(it);
        return true;
    } catch (const std::exception& e) {
        std::cout << "使用卡片时发生错误: " << e.what() << "\n";
        return false;
    }
}



void Player::saveState(std::ofstream& out) const {
    size_t nameLength = name.length();
    out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    out.write(name.c_str(), nameLength);
    
    out.write(reinterpret_cast<const char*>(&actions), sizeof(actions));
    out.write(reinterpret_cast<const char*>(&buys), sizeof(buys));
    out.write(reinterpret_cast<const char*>(&coins), sizeof(coins));
    
    // TODO: 实现牌组序列化
}

void Player::loadState(std::ifstream& in) {
    size_t nameLength;
    in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    name.resize(nameLength);
    in.read(&name[0], nameLength);
    
    in.read(reinterpret_cast<char*>(&actions), sizeof(actions));
    in.read(reinterpret_cast<char*>(&buys), sizeof(buys));
    in.read(reinterpret_cast<char*>(&coins), sizeof(coins));
    
    // TODO: 实现牌组反序列化
}

bool Player::playTreasure(const std::string& cardName) {
    auto it = std::find_if(hand.begin(), hand.end(),
        [&cardName](const auto& card) { return card->getName() == cardName; });
    
    if (it == hand.end()) {
        return false;
    }
    
    if ((*it)->getType() != CardType::TREASURE) {
        return false;
    }
    
    // 创建一个临时Game对象，因为财富卡不使用游戏状态
    static Game dummyGame(0);  // 使用静态对象避免重复创建
    (*it)->play(*this, dummyGame);
    discard.push_back(*it);
    hand.erase(it);
    return true;
}

std::vector<std::shared_ptr<Card>> Player::getTreasureCards() const {
    std::vector<std::shared_ptr<Card>> treasures;
    for (const auto& card : hand) {
        if (card->getType() == CardType::TREASURE) {
            treasures.push_back(card);
        }
    }
    return treasures;
}

void Player::playAllTreasures() {
    std::cout << "\n自动使用所有财富卡:\n";
    
    auto treasures = getTreasureCards();
    if (!treasures.empty()) {
        for (const auto& card : treasures) {
            // 从手牌中移除
            auto it = std::find(hand.begin(), hand.end(), card);
            if (it != hand.end()) {
                // 使用卡片
                card->play(*this, *static_cast<Game*>(nullptr));  // 修改这里，移除创建新游戏的代码
                
                // 移到弃牌堆
                discard.push_back(*it);
                hand.erase(it);
                
                std::cout << "使用了 " << card->getName() << " (+";
                if (card->getName() == "Copper") std::cout << "1";
                else if (card->getName() == "Silver") std::cout << "2";
                else if (card->getName() == "Gold") std::cout << "3";
                std::cout << " 金币)\n";
            }
        }
        
        std::cout << "当前总金币: " << coins << "\n";
    }
}

bool Player::hasMoat() const {
    for (const auto& card : hand) {
        if (card->getName() == "Moat") {
            return true;
        }
    }
    return false;
}

bool Player::trashCopperFromHand() {
    auto it = std::find_if(hand.begin(), hand.end(),
        [](const auto& card) { return card->getName() == "Copper"; });
    
    if (it != hand.end()) {
        trash.push_back(*it);
        hand.erase(it);
        return true;
    }
    return false;
}

void Player::discardDownTo(int count) {
    while (hand.size() > count) {
        std::cout << "当前手牌: ";
        for (size_t i = 0; i < hand.size(); i++) {
            std::cout << i + 1 << "." << hand[i]->getName() << " ";
        }
        
        std::cout << "\n请选择要弃掉的卡片编号: ";
        int choice;
        std::cin >> choice;
        
        if (choice > 0 && choice <= hand.size()) {
            discard.push_back(hand[choice - 1]);
            hand.erase(hand.begin() + choice - 1);
        }
    }
}

int Player::discardAndDraw() {
    std::cout << "当前手牌: ";
    for (size_t i = 0; i < hand.size(); i++) {
        std::cout << i + 1 << "." << hand[i]->getName() << " ";
    }
    
    std::cout << "\n请输入要弃掉的卡片编号（用空格分隔，输入0结束）: ";
    int count = 0;
    std::vector<int> choices;
    int choice;
    
    while (std::cin >> choice && choice != 0) {
        if (choice > 0 && choice <= hand.size()) {
            choices.push_back(choice - 1);
            count++;
        }
    }
    
    // 从大到小排序，以便从后往前删除
    std::sort(choices.rbegin(), choices.rend());
    
    for (int idx : choices) {
        discard.push_back(hand[idx]);
        hand.erase(hand.begin() + idx);
    }
    
    // 抽相同数量的牌
    drawCard(count);
    return count;
}

std::vector<std::shared_ptr<Card>> Player::getAllCards() const {
    std::vector<std::shared_ptr<Card>> allCards;
    // 合并所有牌组中的卡片
    allCards.insert(allCards.end(), deck.begin(), deck.end());
    allCards.insert(allCards.end(), hand.begin(), hand.end());
    allCards.insert(allCards.end(), discard.begin(), discard.end());
    return allCards;
}

void Player::discardCard(int index) {
    if (index >= 0 && index < hand.size()) {
        discard.push_back(hand[index]);
        hand.erase(hand.begin() + index);
    }
}

void Player::discardCellar() {
    auto it = std::find_if(hand.begin(), hand.end(),
        [](const auto& card) { return card->getName() == "Cellar"; });
    
    if (it != hand.end()) {
        discard.push_back(*it);
        hand.erase(it);
    }
}

void Player::showCardDescription(const std::string& cardName) const {
    for (const auto& card : hand) {
        if (Utils::equalIgnoreCase(card->getName(), cardName)) {
            std::cout << "\n" << card->getDescription() << "\n";
            return;
        }
    }
    std::cout << "在手牌中找不到该卡片。\n";
}

void Player::trashCardFromHand(size_t index) {
    if (index < hand.size()) {
        trash.push_back(hand[index]);
        hand.erase(hand.begin() + index);
    }
}

void Player::trashCardFromDeck(size_t index) {
    if (index < deck.size()) {
        trash.push_back(deck[index]);
        deck.erase(deck.begin() + index);
    }
}

// JSON 序列化相关方法实现
json Player::toJson() const {
    json j;
    j["name"] = name;
    j["actions"] = actions;
    j["buys"] = buys;
    j["coins"] = coins;
    
    // 序列化牌组
    j["deck"] = json::array();
    for (const auto& card : deck) {
        j["deck"].push_back(card->getName());
    }
    
    j["hand"] = json::array();
    for (const auto& card : hand) {
        j["hand"].push_back(card->getName());
    }
    
    j["discard"] = json::array();
    for (const auto& card : discard) {
        j["discard"].push_back(card->getName());
    }
    
    j["trash"] = json::array();
    for (const auto& card : trash) {
        j["trash"].push_back(card->getName());
    }
    
    return j;
}

void Player::fromJson(const json& j) {
    name = j["name"].get<std::string>();
    actions = j["actions"].get<int>();
    buys = j["buys"].get<int>();
    coins = j["coins"].get<int>();
    
    // 清空现有牌组
    deck.clear();
    hand.clear();
    discard.clear();
    
    // 加载牌组
    for (const auto& cardJson : j["deck"]) {
        deck.push_back(Card::fromJson(cardJson));
    }
    
    for (const auto& cardJson : j["hand"]) {
        hand.push_back(Card::fromJson(cardJson));
    }
    
    for (const auto& cardJson : j["discard"]) {
        discard.push_back(Card::fromJson(cardJson));
    }
}









