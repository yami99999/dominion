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
    std::cout << "[牌堆状态] 抽牌堆: " << deck.size() << " 张, "
              << "弃牌堆: " << discard.size() << " 张\n";
}

void Player::showHand() const {
    std::cout << "手牌 (" << hand.size() << " 张): ";
    bool first = true;
    
    // 按类型分类显示
    // 首先显示财宝卡
    for (const auto& card : hand) {
        if (card->getType() == CardType::TREASURE) {
            if (!first) std::cout << ", ";
            std::cout << "\033[33m" << card->getName() << "\033[0m";  // 黄色显示财宝卡
            first = false;
        }
    }
    
    // 然后显示行动卡
    for (const auto& card : hand) {
        if (card->getType() == CardType::ACTION) {
            if (!first) std::cout << ", ";
            std::cout << "\033[32m" << card->getName() << "\033[0m";  // 绿色显示行动卡
            first = false;
        }
    }
    
    // 最后显示胜利点数卡和诅咒卡
    for (const auto& card : hand) {
        if (card->getType() == CardType::VICTORY) {
            if (!first) std::cout << ", ";
            if (card->getName() == "Curse") {
                std::cout << "\033[35m" << card->getName() << "\033[0m";  // 紫色显示诅咒卡
            } else {
                std::cout << "\033[34m" << card->getName() << "\033[0m";  // 蓝色显示胜利点数卡
            }
            first = false;
        }
    }
    
    std::cout << "\n";
}

bool Player::hasActionCard() const {
    return std::any_of(hand.begin(), hand.end(),
        [](const auto& card) { return card->getType() == CardType::ACTION; });
}

bool Player::playAction(const std::string& cardName, Game& game) {
    auto it = std::find_if(hand.begin(), hand.end(),
        [&cardName](const auto& card) { return card->getName() == cardName; });
    
    if (it == hand.end()) {
        std::cout << "找不到该卡片!\n";
        return false;
    }
    
    if ((*it)->getType() != CardType::ACTION) {
        std::cout << "这不是行动卡!\n";
        return false;
    }
    
    (*it)->play(*this, game);
    discard.push_back(*it);
    hand.erase(it);
    return true;
}

void Player::addToDiscard(std::shared_ptr<Card> card) {
    discard.push_back(card);
}

void Player::saveState(std::ofstream& out) const {
    // 保存玩家名称
    size_t nameLength = name.length();
    out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    out.write(name.c_str(), nameLength);
    
    // 保存资源数量
    out.write(reinterpret_cast<const char*>(&actions), sizeof(actions));
    out.write(reinterpret_cast<const char*>(&buys), sizeof(buys));
    out.write(reinterpret_cast<const char*>(&coins), sizeof(coins));
    
    // 保存牌组信息
    // TODO: 实现牌组序列化
}

void Player::loadState(std::ifstream& in) {
    // 读取玩家名称
    size_t nameLength;
    in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    name.resize(nameLength);
    in.read(&name[0], nameLength);
    
    // 读取资源数量
    in.read(reinterpret_cast<char*>(&actions), sizeof(actions));
    in.read(reinterpret_cast<char*>(&buys), sizeof(buys));
    in.read(reinterpret_cast<char*>(&coins), sizeof(coins));
    
    // 读取牌组信息
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
    auto treasures = getTreasureCards();
    if (!treasures.empty()) {
        std::cout << "自动使用所有财富卡:\n";
    }
    
    for (const auto& card : treasures) {
        // 从手牌中移除
        auto it = std::find(hand.begin(), hand.end(), card);
        if (it != hand.end()) {
            // 使用卡片
            static Game dummyGame(0);  // 财富卡不需要游戏状态
            card->play(*this, dummyGame);
            
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
    
    if (!treasures.empty()) {
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
    // 找到并弃置Cellar
    auto it = std::find_if(hand.begin(), hand.end(),
        [](const auto& card) { return card->getName() == "Cellar"; });
    
    if (it != hand.end()) {
        discard.push_back(*it);
        hand.erase(it);
    }
}