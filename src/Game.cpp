#include "Game.h"
#include "AllCards.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include "Utils.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 定义所有可用的王国卡
const std::vector<std::string> Game::ALL_KINGDOM_CARDS = {
    "Village", "Woodcutter", "Militia", "Market",
    "Smithy", "CouncilRoom", "Moneylender", "Moat",
    "Workshop", "Cellar", "Witch", "Thief",
    "Gardens", "Chapel", "Feast", "Laboratory"
};

Game::Game(int numPlayers, bool randomKingdom, const std::string& kingdomSet) 
    : currentPlayer(0), turnCount(1) {
    
    std::cout << "\n=== 创建新游戏 ===\n";
    std::cout << "玩家数量: " << numPlayers << "\n";
    
    // 创建玩家
    for (int i = 0; i < numPlayers; i++) {
        std::string playerName;
        std::cout << "请输入玩家 " << (i + 1) << " 的名字: ";
        std::getline(std::cin, playerName);
        players.emplace_back(playerName);
    }
    
    // 根据参数选择王国卡初始化方式
    std::vector<std::string> selectedCards;
    if (randomKingdom) {
        selectedCards = getRandomKingdomCards();
    } else if (!kingdomSet.empty()) {
        selectedCards = getPresetKingdomCards(kingdomSet);
    } else {
        selectedCards = selectKingdomCards();
    }
    
    // 记录选择的王国卡
    logger.logKingdomCardSelection(selectedCards);
    
    // 初始化游戏
    initialize(selectedCards);
}

std::vector<std::string> Game::getPresetKingdomCards(const std::string& setName) {
    const std::map<std::string, std::vector<std::string>> PRESET_SETS = {
        {"beginner", {
            "Village", "Cellar", "Moat", "Woodcutter", "Workshop",
            "Militia", "Market", "Smithy", "CouncilRoom", "Laboratory"
        }},
        {"advanced", {
            "Village", "Chapel", "Witch", "Gardens", "Laboratory",
            "Market", "Smithy", "Militia", "Moneylender", "Thief"
        }},
        {"random", getRandomKingdomCards()}
    };
    
    auto it = PRESET_SETS.find(Utils::toLower(setName));
    if (it != PRESET_SETS.end()) {
        return it->second;
    }
    
    std::cout << "未知的预设套装，使用随机选择\n";
    return getRandomKingdomCards();
}

void Game::initialize() {
    // 默认初始化，使用随机选择的王国卡
    initialize(getRandomKingdomCards());
}

std::vector<std::string> Game::selectPresetCards() {
    Utils::printHeader("预设套装选择");
    std::cout << "可用的预设套装:\n";
    std::cout << "1. 初学者套装\n";
    std::cout << "2. 进阶套装\n";
    std::cout << "3. 随机套装\n";
    
    int choice;
    std::cout << "\n请选择套装 (1-3): ";
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    switch (choice) {
        case 1:
            return std::vector<std::string>{
                "Village", "Cellar", "Moat", "Woodcutter", "Workshop",
                "Militia", "Market", "Smithy", "CouncilRoom", "Laboratory"
            };
        case 2:
            return std::vector<std::string>{
                "Village", "Chapel", "Witch", "Gardens", "Laboratory",
                "Market", "Smithy", "Militia", "Moneylender", "Thief"
            };
        case 3:
        default:
            return getRandomKingdomCards();
    }
}

void Game::initialize(const std::vector<std::string>& selectedCards) {
    try {
        // 初始化基础卡供应堆
        const std::vector<std::pair<std::string, int>> basicCards = {
            {"Copper", 60},    // 铜币 - 60张
            {"Silver", 40},    // 银币 - 40张
            {"Gold", 30},      // 金币 - 30张
            {"Estate", players.size() <= 2 ? 8 + players.size()*3 : 12 + players.size()*3},    // 庄园(!!难以修复, 所以直接加玩家数量*3)
            {"Duchy", players.size() <= 2 ? 8 : 12},     // 公爵领地
            {"Province", players.size() <= 2 ? 8 : 12},  // 行省
            {"Curse", players.size() == 2 ? 10 : 
                     (players.size() == 3 ? 20 : 30)}    // 诅咒卡
        };

        // 初始化基础卡
        for (const auto& [cardName, count] : basicCards) {
            supply[cardName] = std::vector<std::shared_ptr<Card>>();
            for (int i = 0; i < count; i++) {
                supply[cardName].push_back(Card::createCard(cardName));
            }
        }
        
        // 初始化选中的王国卡
        for (const auto& cardName : selectedCards) {
            try {
                supply[cardName] = std::vector<std::shared_ptr<Card>>();
                for (int i = 0; i < 10; i++) {
                    supply[cardName].push_back(Card::createCard(cardName));
                }
            } catch (const std::exception& e) {
                std::cerr << "初始化卡片失败: " << cardName << " - " << e.what() << std::endl;
                throw std::runtime_error("初始化王国卡失败: " + cardName);
            }
        }
        
        // 为每个玩家初始化牌组
        for (auto& player : players) {
            // 从供应堆中取出初始卡牌
            for (int i = 0; i < 7; i++) {  // 7张铜币
                if (!supply["Copper"].empty()) {
                    player.addToDiscard(supply["Copper"].back());
                    supply["Copper"].pop_back();
                }
            }
            for (int i = 0; i < 3; i++) {  // 3张庄园
                if (!supply["Estate"].empty()) {
                    player.addToDiscard(supply["Estate"].back());
                    supply["Estate"].pop_back();
                }
            }
            player.shuffleDeck();
            player.drawCard(5); // 抽取起始手牌
        }
        
    } catch (const std::exception& e) {
        std::cerr << "游戏初始化失败: " << e.what() << std::endl;
        throw;
    }
}

void Game::playGame() {
    while (!isGameOver()) {
        playTurn();
        
        // 回合结束后的命令处理循环
        while (true) {
            std::cout << "\n输入 'save' 保存游戏，'stats' 查看统计信息，'go' 继续游戏: ";
            std::string command;
            std::getline(std::cin, command);
            
            if (Utils::equalIgnoreCase(command, "save")) {
                std::cout << "请输入存档文件名 (直接回车使用默认名称 'dominion_save.txt'): ";
                std::string filename;
                std::getline(std::cin, filename);
                
                if (filename.empty()) {
                    filename = "dominion_save.txt";
                } else if (filename.find(".txt") == std::string::npos) {
                    filename += ".txt";
                }
                
                // 检查文件是否已存在
                std::ifstream checkFile(filename);
                if (checkFile.good()) {
                    checkFile.close();
                    std::cout << "文件 '" << filename << "' 是否确认保存? (y/n): ";
                    std::string response;
                    std::getline(std::cin, response);
                    
                    if (!Utils::equalIgnoreCase(response, "y") && 
                        !Utils::equalIgnoreCase(response, "yes")) {
                        std::cout << "取消保存\n";
                        continue;  // 返回命令输入循环
                    }
                }
                
                saveGame(filename);
                continue;  // 返回命令输入循环
            } 
            else if (Utils::equalIgnoreCase(command, "stats")) {
                displayStats();
                continue;  // 返回命令输入循环
            }
            else if (Utils::equalIgnoreCase(command, "go")) {
                break;  // 退出命令输入循环，继续游戏
            }
            else if (!command.empty()) {
                std::cout << "无效的命令。请输入 'save'、'stats' 或 'go'\n";
                continue;  // 返回命令输入循环
            }
            // 如果输入为空，继续循环
        }
    }
    
    // 游戏结束时自动保存
    std::time_t now = std::time(nullptr);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y%m%d_%H%M%S", localtime(&now));
    std::string autoSaveFilename = "dominion_autosave_" + std::string(timeStr) + ".txt";
    
    std::cout << "\n游戏结束，正在创建自动存档...\n";
    saveGame(autoSaveFilename);
    
    // 显示最终结果
    auto scores = calculateFinalScores();
    displayResults(scores);
}

void Game::playTurn() {
    Player& currentPlayerRef = players[currentPlayer];
    
    std::cout << "\n========== 回合 " << turnCount << " - " 
              << currentPlayerRef.getName() << " ==========\n";
    
    // 显示牌堆状态
    currentPlayerRef.showDeckStatus();
    
    // 显示当前手牌
    std::cout << "[" << currentPlayerRef.getName() << "] ";
    currentPlayerRef.showHand();
    
    // 行动阶段
    playActionPhase();
    
    // 购买阶段
    playBuyPhase();
    
    // 清理阶段
    playCleanupPhase();
    
    // 切换到下一个玩家
    currentPlayer = (currentPlayer + 1) % players.size();
    
    // 如果回到第一个玩家，回合数加1
    if (currentPlayer == 0) {
        turnCount++;
    }
}

void Game::playActionPhase() {
    Player& currentPlayerRef = players[currentPlayer];
    
    std::cout << "\n[回合 " << turnCount << " - 行动阶段]\n";
    
    while (currentPlayerRef.getActions() > 0) {
        std::cout << "[回合 " << turnCount << "] " 
                 << currentPlayerRef.getName() 
                 << " 的行动点数: " << currentPlayerRef.getActions() << "\n\n";
        
        // 显示当前手牌
        currentPlayerRef.showHand();
        std::cout << "\n";
        
        if (!currentPlayerRef.hasActionCard()) {
            std::cout << "没有行动卡可以使用\n";
            break;
        }
        
        std::cout << "[回合 " << turnCount << "] " 
                 << currentPlayerRef.getName() 
                 << " 选择要使用的行动卡 ('show' 查看手牌, 'info 卡片名称' 查看说明，'skip' 跳过): ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (Utils::equalIgnoreCase(input, "skip")) {
            break;
        }
        else if (Utils::equalIgnoreCase(input, "show")) {
            currentPlayerRef.showHand();
            std::cout << "\n";
            continue;
        }
        else if (input.substr(0, 4) == "info") {
            std::string cardName = input.substr(5);
            currentPlayerRef.showCardDescription(cardName);
            continue;
        }
        
        // 尝试使用行动卡
        try {
            if (currentPlayerRef.playAction(input, *this)) {
                currentPlayerRef.useAction();
                logger.recordCardPlayed(input);
            }
        } catch (const std::exception& e) {
            std::cout << "使用行动卡失败: " << e.what() << "\n";
            continue;
        }
    }
}

void Game::playBuyPhase() {
    Player& currentPlayerRef = players[currentPlayer];
    
    std::cout << "\n=== " << currentPlayerRef.getName() << " 的购买阶段 ===\n";
    currentPlayerRef.playAllTreasures();
    
    while (currentPlayerRef.getBuys() > 0) {
        int availableCoins = currentPlayerRef.getCoins();
        std::cout << "\n[" << currentPlayerRef.getName() << "] 剩余购买次数: " 
                 << currentPlayerRef.getBuys() << "\n";
        
        // 显示可购买的卡片
        displaySupplyPiles();
        
        // 显示可用金币
        std::cout << "\n[" << currentPlayerRef.getName() << "] 可用金币: " 
                 << availableCoins << "\n";
        
        std::cout << "[" << currentPlayerRef.getName() 
                 << "] 请输入卡片名称进行购买，或输入 'done' 结束购买阶段: ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (Utils::equalIgnoreCase(input, "done")) break;
        
        auto it = std::find_if(supply.begin(), supply.end(),
            [&input](const auto& pair) {
                return Utils::equalIgnoreCase(pair.first, input);
            });
            
        if (it != supply.end() && !it->second.empty()) {
            int cardCost = it->second[0]->getCost();
            if (cardCost <= availableCoins) {
                // 购买卡片
                std::shared_ptr<Card> boughtCard = it->second.back();
                it->second.pop_back();
                currentPlayerRef.addToDiscard(boughtCard);
                currentPlayerRef.spendCoins(cardCost);
                currentPlayerRef.useBuy();
                
                logger.recordCardBought(it->first);
                logger.logAction(currentPlayerRef.getName(), 
                    "购买了 " + it->first + " (花费: " + std::to_string(cardCost) + " 金币)");
                
                std::cout << "成功购买 " << it->first << "!\n";
                
                if (currentPlayerRef.getBuys() == 0) {
                    std::cout << "没有剩余购买次数了\n";
                    break;
                }
                if (currentPlayerRef.getCoins() == 0) {
                    std::cout << "没有剩余金币了\n";
                    break;
                }
            } else {
                std::cout << "金币不足！需要 " << cardCost << " 金币\n";
            }
        } else {
            if (it == supply.end()) {
                std::cout << "找不到该卡片\n";
            } else {
                std::cout << "该卡片已售罄\n";
            }
        }
    }
}

void Game::playCleanupPhase() {
    Player& currentPlayerRef = players[currentPlayer];
    currentPlayerRef.discardHand();
    currentPlayerRef.drawCard(5);
    currentPlayerRef.resetActions();
    currentPlayerRef.resetBuys();
    currentPlayerRef.resetCoins();
}

bool Game::isGameOver() const {
    // 1. 检查Province堆是否为空
    if (isProvinceEmpty()) {
        std::cout << "\n*** 游戏结束: Province 已耗尽! ***\n";
        return true;
    }
    
    // 2. 检查是否有任意三堆卡牌堆空（括所有类型的卡）
    int emptyPiles = getEmptyPiles();
    if (emptyPiles >= 3) {
        std::cout << "\n*** 游戏结束: 已有 " << emptyPiles << " 个供应堆耗尽! ***\n";
        return true;
    }
    
    return false;
}

bool Game::isProvinceEmpty() const {
    auto it = supply.find("Province");
    return it == supply.end() || it->second.empty();
}

int Game::getEmptyPiles() const {
    int emptyPiles = 0;
    
    // 检查所有供应堆，包括基础卡和王国卡
    for (const auto& [cardName, pile] : supply) {
        if (pile.empty()) {
            emptyPiles++;
        }
    }
    
    return emptyPiles;
}

void Game::saveGame(const std::string& filename) {
    try {
        json saveData;
        
        // 保存基本游戏信息
        saveData["turn_count"] = turnCount;
        saveData["current_player"] = currentPlayer;
        
        // 保存玩家信息
        json playersJson = json::array();
        for (const auto& player : players) {
            playersJson.push_back(player.toJson());
        }
        saveData["players"] = playersJson;
        
        // 保存供应堆状态
        json supplyJson;
        for (const auto& [cardName, pile] : supply) {
            supplyJson[cardName] = pile.size();
        }
        saveData["supply"] = supplyJson;
        
        // 保存游戏日志
        saveData["game_log"] = logger.toJson();
        
        // 写入文件
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("无法创建存档文件: " + filename);
        }
        out << std::setw(4) << saveData << std::endl;
        
        std::cout << "游戏已成功保存到 " << filename << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "保存游戏时发生错误: " << e.what() << "\n";
        throw;
    }
}

void Game::loadGame(const std::string& filename) {
    std::cout << "\n正在加载游戏...\n";
    
    try {
        // 读取文件
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("无法打开存档文件: " + filename);
        }
        
        // 解析JSON
        json saveData;
        try {
            in >> saveData;
        } catch (const json::exception& e) {
            throw std::runtime_error("解析存档文件失败: " + std::string(e.what()));
        }
        
        // 加载基本信息
        turnCount = saveData["turn_count"].get<int>();
        currentPlayer = saveData["current_player"].get<int>();
        
        // 加载玩家信息
        players.clear();
        for (const auto& playerJson : saveData["players"]) {
            try {
                std::string playerName = playerJson["name"].get<std::string>();
                Player player(playerName);
                
                // 设置玩家状态
                player.setActions(playerJson["actions"].get<int>());
                player.setBuys(playerJson["buys"].get<int>());
                player.setCoins(playerJson["coins"].get<int>());
                
                // 加载牌组
                for (const auto& cardName : playerJson["deck"]) {
                    try {
                        player.addToDeck(Card::createCard(cardName.get<std::string>()));
                    } catch (const std::exception& e) {
                        throw std::runtime_error("加载牌组失败: " + std::string(e.what()));
                    }
                }
                
                // 加载手牌
                for (const auto& cardName : playerJson["hand"]) {
                    try {
                        player.addToHand(Card::createCard(cardName.get<std::string>()));
                    } catch (const std::exception& e) {
                        throw std::runtime_error("加载手牌失败: " + std::string(e.what()));
                    }
                }
                
                // 加载弃牌堆
                for (const auto& cardName : playerJson["discard"]) {
                    try {
                        player.addToDiscard(Card::createCard(cardName.get<std::string>()));
                    } catch (const std::exception& e) {
                        throw std::runtime_error("加载弃牌堆失败: " + std::string(e.what()));
                    }
                }
                
                players.push_back(std::move(player));
            } catch (const std::exception& e) {
                throw std::runtime_error("加载玩家数据失败: " + std::string(e.what()));
            }
        }
        
        // 加载供应堆
        supply.clear();
        for (const auto& [cardName, count] : saveData["supply"].items()) {
            try {
                supply[cardName] = std::vector<std::shared_ptr<Card>>();
                for (int i = 0; i < count.get<int>(); i++) {
                    supply[cardName].push_back(Card::createCard(cardName));
                }
            } catch (const std::exception& e) {
                throw std::runtime_error("加载供应堆失败: " + cardName + " - " + e.what());
            }
        }
        
        // 加载游戏日志
        if (saveData.contains("game_log")) {
            logger.fromJson(saveData["game_log"]);
        }
        
        std::cout << "游戏已从 " << filename << " 加载\n";
        std::cout << "当前回合: " << turnCount << "\n";
        std::cout << "当前玩家: " << players[currentPlayer].getName() << "\n";
        
    } catch (const std::exception& e) {
        throw std::runtime_error("加载游戏失败: " + std::string(e.what()));
    }
}

std::vector<std::pair<std::string, int>> Game::calculateFinalScores() const {
    std::vector<std::pair<std::string, int>> scores;
    
    for (const auto& player : players) {
        int totalScore = 0;
        for (const auto& card : player.getAllCards()) {
            totalScore += card->getVictoryPoints();
        }
        scores.emplace_back(player.getName(), totalScore);
    }
    
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return scores;
}

void Game::displayResults(const std::vector<std::pair<std::string, int>>& scores) {
    std::cout << "\n=== 游戏束 ===\n\n";
    std::cout << "最终得分:\n";
    
    int highestScore = scores[0].second;
    for (const auto& [name, score] : scores) {
        std::cout << name << ": " << score << " 点";
        if (score == highestScore) {
            std::cout << " 🏆";
        }
        std::cout << "\n";
    }
}

void Game::displayFinalState() const {
    std::cout << "\n=== 游戏结束状态 ===\n";
    std::cout << "总回合数: " << turnCount << "\n";
    std::cout << "空的供应堆:\n";
    
    for (const auto& [cardName, pile] : supply) {
        if (pile.empty()) {
            std::cout << "- " << cardName << "\n";
        }
    }
    std::cout << "==================\n\n";
}

void Game::displayStats() {
    std::cout << "\n=== 游戏统计信息 ===\n";
    
    // 显示供应堆状态
    std::cout << "供应堆状态:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "卡片名称" 
              << std::setw(10) << "剩余数量" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    // 按类型分组显示
    // 首先示基础财宝卡
    for (const auto& [cardName, pile] : supply) {
        if (cardName == "Copper" || cardName == "Silver" || cardName == "Gold") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    
    // 然后显示胜利点数卡和诅咒卡
    for (const auto& [cardName, pile] : supply) {
        if (cardName == "Estate" || cardName == "Duchy" || 
            cardName == "Province" || cardName == "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    
    // 最后显示王国卡
    for (const auto& [cardName, pile] : supply) {
        if (cardName != "Copper" && cardName != "Silver" && cardName != "Gold" &&
            cardName != "Estate" && cardName != "Duchy" && 
            cardName != "Province" && cardName != "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    
    std::cout << std::string(40, '-') << "\n";
    
    // 显示每个玩家的牌堆状态
    std::cout << "\n玩家牌堆状态:\n";
    for (const auto& player : players) {
        std::cout << player.getName() << ": ";
        player.showDeckStatus();
    }
}

void Game::otherPlayersDiscardToThree(const Player& currentPlayer) {
    for (auto& player : players) {
        if (&player != &currentPlayer) {
            // 检查是否有护城河防御
            if (!player.hasMoat()) {
                int handSize = player.getHandSize();
                if (handSize > 3) {
                    std::cout << "\n" << player.getName() << " 需要弃掉 " 
                             << (handSize - 3) << " 张牌\n";
                    player.discardDownTo(3);
                }
            } else {
                std::cout << player.getName() << " 使用护城河防御了攻击\n";
                logger.logAction(player.getName(), "使用护城河防御了攻击");
            }
        }
    }
}

void Game::otherPlayersDraw(const Player& currentPlayer, int count) {
    for (auto& player : players) {
        if (&player != &currentPlayer) {
            player.drawCard(count);
            logger.logAction(player.getName(), 
                "抽了 " + std::to_string(count) + " 张牌");
        }
    }
}

void Game::gainCardUpToCost(Player& player, int maxCost) {
    std::cout << "\n=== 获得一张卡片 ===\n";
    std::cout << "可获得的卡片 (最大花费: " << maxCost << "):\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "卡片名称" 
             << std::setw(10) << "花费" 
             << std::setw(10) << "剩余数量" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    // 显示可获得的卡片
    for (const auto& [cardName, pile] : supply) {
        if (!pile.empty() && pile[0]->getCost() <= maxCost) {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile[0]->getCost()
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    std::cout << std::string(40, '-') << "\n";
    
    std::cout << "\n请输入要获得的卡片名称: ";
    std::string input;
    std::getline(std::cin, input);
    
    auto it = std::find_if(supply.begin(), supply.end(),
        [&input](const auto& pair) {
            return Utils::equalIgnoreCase(pair.first, input);
        });
        
    if (it != supply.end() && !it->second.empty() && 
        it->second[0]->getCost() <= maxCost) {
        std::shared_ptr<Card> gainedCard = it->second.back();
        it->second.pop_back();
        player.addToDiscard(gainedCard);
        logger.logAction(player.getName(), "获得 " + it->first);
        std::cout << "成功获得 " << it->first << "!\n";
    } else {
        std::cout << "无法获得卡片\n";
    }
}

void Game::logAction(const std::string& playerName, const std::string& action) {
    logger.logAction(playerName, action);
}

void Game::showSupplyCardDescription(const std::string& cardName) const {
    auto it = supply.find(cardName);
    if (it != supply.end() && !it->second.empty()) {
        std::cout << "\n" << it->second[0]->getDescription() << "\n";
    } else {
        std::cout << "在供应堆中找不到该卡片。\n";
    }
}

void Game::displaySupplyPiles() const {
    std::cout << "\n=== 供应堆状态 ===\n";
    
    // 显示金币卡
    std::cout << "\n财宝卡:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "卡片名称" 
              << std::setw(10) << "花费" 
              << std::setw(10) << "剩余数量" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    const std::vector<std::string> treasureCards = {"Copper", "Silver", "Gold"};
    for (const auto& cardName : treasureCards) {
        if (supply.count(cardName) && !supply.at(cardName).empty()) {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << supply.at(cardName)[0]->getCost()
                     << std::setw(10) << supply.at(cardName).size() << "\n";
        }
    }
    
    // 显示胜利点数卡
    std::cout << "\n胜利点数卡:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "卡片名称" 
              << std::setw(10) << "花费" 
              << std::setw(10) << "剩余数量" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    const std::vector<std::string> victoryCards = {"Estate", "Duchy", "Province"};
    for (const auto& cardName : victoryCards) {
        if (supply.count(cardName) && !supply.at(cardName).empty()) {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << supply.at(cardName)[0]->getCost()
                     << std::setw(10) << supply.at(cardName).size() << "\n";
        }
    }
    
    // 显示诅咒卡
    std::cout << "\n诅咒卡:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "卡片名称" 
              << std::setw(10) << "花费" 
              << std::setw(10) << "剩余数量" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    if (supply.count("Curse") && !supply.at("Curse").empty()) {
        std::cout << std::setw(15) << "Curse"
                 << std::setw(10) << supply.at("Curse")[0]->getCost()
                 << std::setw(10) << supply.at("Curse").size() << "\n";
    }
    
    // 显示行动卡
    std::cout << "\n动卡:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "卡片名称" 
              << std::setw(10) << "花费" 
              << std::setw(10) << "剩余数量" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    for (const auto& [cardName, pile] : supply) {
        // 如果不是基础卡或诅咒卡，就是行动卡
        if (!pile.empty() && 
            std::find(treasureCards.begin(), treasureCards.end(), cardName) == treasureCards.end() &&
            std::find(victoryCards.begin(), victoryCards.end(), cardName) == victoryCards.end() &&
            cardName != "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile[0]->getCost()
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    std::cout << std::string(40, '-') << "\n";
}

void Game::otherPlayersGainCurse(const Player& currentPlayer) {
    for (auto& player : players) {
        if (&player != &currentPlayer) {
            // 检查是否有护城河防御
            if (!player.hasMoat()) {
                // 检查诅咒牌堆是否还有牌
                if (!supply["Curse"].empty()) {
                    player.addToDiscard(supply["Curse"].back());
                    supply["Curse"].pop_back();
                    logger.logAction(player.getName(), "获得了一张诅咒卡");
                }
            } else {
                logger.logAction(player.getName(), "使用护城河防御了女巫的效果");
            }
        }
    }
}

void Game::resolveThiefEffect(Player& currentPlayer) {
    for (auto& player : players) {
        if (&player != &currentPlayer) {
            if (!player.hasMoat()) {
                // 查看牌库顶的两张牌
                std::vector<std::shared_ptr<Card>> revealedCards;
                auto& playerDeck = player.getDeck();
                
                for (int i = 0; i < 2 && !playerDeck.empty(); i++) {
                    revealedCards.push_back(playerDeck.back());
                    playerDeck.pop_back();
                }
                
                // 显示被揭示的牌
                std::cout << player.getName() << " 揭示的牌: ";
                for (const auto& card : revealedCards) {
                    std::cout << card->getName() << " ";
                }
                std::cout << "\n";
                
                // 找出财宝牌
                std::vector<std::shared_ptr<Card>> treasures;
                std::copy_if(revealedCards.begin(), revealedCards.end(),
                    std::back_inserter(treasures),
                    [](const auto& card) { return card->getType() == CardType::TREASURE; });
                
                // 如果有财宝牌，让当前玩家选择一张
                if (!treasures.empty()) {
                    std::cout << "选择要获得的财宝牌 (输入编号):\n";
                    for (size_t i = 0; i < treasures.size(); i++) {
                        std::cout << i + 1 << ". " << treasures[i]->getName() << "\n";
                    }
                    
                    int choice;
                    std::cin >> choice;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    
                    if (choice > 0 && choice <= treasures.size()) {
                        currentPlayer.addToDiscard(treasures[choice - 1]);
                        logger.logAction(currentPlayer.getName(), 
                            "从 " + player.getName() + " 获得了 " + treasures[choice - 1]->getName());
                    }
                }
                
                // 其余的牌放入弃牌堆
                for (const auto& card : revealedCards) {
                    player.addToDiscard(card);
                }
            }
        }
    }
}

void Game::resolveChapelEffect(Player& currentPlayer) {
    int trashCount = 0;
    while (trashCount < 4 && !currentPlayer.getHand().empty()) {
        std::cout << "当前手牌:\n";
        const auto& hand = currentPlayer.getHand();
        for (size_t i = 0; i < hand.size(); i++) {
            std::cout << i + 1 << ". " << hand[i]->getName() << "\n";
        }
        
        std::cout << "选择要废除的牌 (输入0结束): ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (choice == 0) break;
        
        if (choice > 0 && choice <= hand.size()) {
            std::string cardName = hand[choice - 1]->getName();
            currentPlayer.trashCardFromHand(choice - 1);
            trashCount++;
            logger.logAction(currentPlayer.getName(), "废除了 " + cardName);
        }
    }
}

int Game::calculateGardensPoints(const Player& player) const {
    int totalCards = player.getAllCards().size();
    return totalCards / 10;  // 每10张牌价值1分
}

void Game::displayKingdomCardInfo(const std::string& cardName) const {
    auto card = Card::createCard(cardName);
    Utils::printDivider();
    std::cout << card->getDescription() << "\n";
    Utils::printDivider();
}

std::vector<std::string> Game::selectKingdomCards() {
    Utils::printHeader("选择王国卡");
    std::cout << "请选择初始化王国卡的方式:\n";
    std::cout << "1. 随机选择10张\n";
    std::cout << "2. 手动选择\n";
    std::cout << "3. 使用推荐套装\n";
    std::cout << "4. 查看卡片详细信息\n";
    
    std::vector<std::string> selected;
    while (true) {
        std::cout << "\n请输入选择 (1-4): ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                selected = getRandomKingdomCards();
                break;
            case 2:
                selected = manualSelectCards();
                break;
            case 3:
                selected = selectPresetCards();
                break;
            case 4:
                viewCardDetails();
                continue;
            default:
                std::cout << "无效的选择，请重试\n";
                continue;
        }
        
        if (selected.size() == 10) {
            break;
        }
    }
    
    // 显示选择结果
    Utils::printHeader("已选择的王国卡");
    for (const auto& cardName : selected) {
        std::cout << "- " << cardName << "\n";
    }
    
    return selected;
}

std::vector<std::string> Game::manualSelectCards() {
    std::vector<std::string> selected;
    displayAvailableKingdomCards();
    
    std::cout << "\n请依次输入10张卡片的编号 (输入-1查看卡片详情，-2重新选择):\n";
    while (selected.size() < 10) {
        std::cout << "还需选择 " << (10 - selected.size()) << " 张卡片: ";
        int index;
        std::cin >> index;
        
        if (index == -1) {
            std::cout << "输入卡片编号查看详情: ";
            int cardIndex;
            std::cin >> cardIndex;
            if (cardIndex >= 0 && cardIndex < ALL_KINGDOM_CARDS.size()) {
                displayKingdomCardInfo(ALL_KINGDOM_CARDS[cardIndex]);
            }
            continue;
        }
        
        if (index == -2) {
            selected.clear();
            continue;
        }
        
        if (index >= 0 && index < ALL_KINGDOM_CARDS.size()) {
            // 检查是否已选择
            if (std::find(selected.begin(), selected.end(), 
                ALL_KINGDOM_CARDS[index]) != selected.end()) {
                std::cout << "该卡片已被选择，请选择其他卡片\n";
                continue;
            }
            selected.push_back(ALL_KINGDOM_CARDS[index]);
        } else {
            std::cout << "无效的选择，请重试\n";
        }
    }
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return selected;
}

void Game::viewCardDetails() {
    displayAvailableKingdomCards();
    std::cout << "\n输入卡片编号查看详情 (输入-1返回): ";
    
    while (true) {
        int index;
        std::cin >> index;
        
        if (index == -1) break;
        
        if (index >= 0 && index < ALL_KINGDOM_CARDS.size()) {
            displayKingdomCardInfo(ALL_KINGDOM_CARDS[index]);
        } else {
            std::cout << "无效的编号，请重试\n";
        }
        
        std::cout << "\n输入卡片编号查看详情 (输入-1返回): ";
    }
}

std::vector<std::string> Game::getRandomKingdomCards(int count) {
    try {
        std::vector<std::string> availableCards;
        // 只包含已实现的卡片
        for (const auto& cardName : ALL_KINGDOM_CARDS) {
            try {
                // 测试是否可以创建卡片
                Card::createCard(cardName);
                availableCards.push_back(cardName);
            } catch (...) {
                std::cerr << "警告: 跳过未实现的卡片: " << cardName << std::endl;
            }
        }
        
        if (availableCards.size() < count) {
            throw std::runtime_error("可用的王国卡数量不足");
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(availableCards.begin(), availableCards.end(), gen);
        availableCards.resize(count);
        return availableCards;
        
    } catch (const std::exception& e) {
        std::cerr << "随机选择王国卡失败: " << e.what() << std::endl;
        throw;
    }
}

void Game::displayAvailableKingdomCards() const {
    std::cout << "\n可用的王国卡:\n";
    for (size_t i = 0; i < ALL_KINGDOM_CARDS.size(); i++) {
        std::cout << i << ". " << ALL_KINGDOM_CARDS[i] << "\n";
    }
}

void Game::initializeKingdomCards(const std::vector<std::string>& selectedCards) {
    std::cout << "\n选择的王国卡:\n";
    for (const auto& cardName : selectedCards) {
        supply[cardName] = std::vector<std::shared_ptr<Card>>();
        for (int i = 0; i < 10; i++) {
            supply[cardName].push_back(Card::createCard(cardName));
        }
        std::cout << "- " << cardName << " (10张)\n";
    }
}

void Game::displayGameState() const {
    Utils::printHeader("游戏状态");
    std::cout << "回合数: " << turnCount << "\n";
    std::cout << "当前玩家: " << players[currentPlayer].getName() << "\n";
    
    // 显示供应堆状态
    displaySupplyStatus();
    
    // 显示所有玩家状态
    Utils::printHeader("玩家状态");
    for (const auto& player : players) {
        displayPlayerStatus(player);
    }
}

void Game::displaySupplyStatus() const {
    Utils::printHeader("供应堆状态");
    
    // 显示基础卡
    std::cout << "基础卡:\n";
    Utils::printDivider('-', 30);
    for (const auto& [cardName, pile] : supply) {
        if (cardName == "Copper" || cardName == "Silver" || cardName == "Gold" ||
            cardName == "Estate" || cardName == "Duchy" || cardName == "Province" ||
            cardName == "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(5) << pile.size() << " 张\n";
        }
    }
    
    // 显示王国卡
    std::cout << "\n王国卡:\n";
    Utils::printDivider('-', 30);
    for (const auto& [cardName, pile] : supply) {
        if (cardName != "Copper" && cardName != "Silver" && cardName != "Gold" &&
            cardName != "Estate" && cardName != "Duchy" && cardName != "Province" &&
            cardName != "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(5) << pile.size() << " 张\n";
        }
    }
}

void Game::displayPlayerStatus(const Player& player) const {
    std::cout << "\n玩家: " << player.getName() << "\n";
    Utils::printDivider('-', 30);
    std::cout << "手牌数: " << player.getHandSize() << "\n";
    std::cout << "牌堆数: " << player.getDeck().size() << "\n";
    std::cout << "弃牌堆: " << player.getDiscard().size() << "\n";
    
    if (&player == &players[currentPlayer]) {
        std::cout << "行动点: " << player.getActions() << "\n";
        std::cout << "购买次数: " << player.getBuys() << "\n";
        std::cout << "金币: " << player.getCoins() << "\n";
        
        std::cout << "\n当前手牌:\n";
        player.showHand();
    }
}

void Game::displayCurrentPlayerStatus() const {
    const Player& currentPlayerRef = players[currentPlayer];
    Utils::printHeader(currentPlayerRef.getName() + " 的状态");
    displayPlayerStatus(currentPlayerRef);
}

std::string Game::getKingdomCardSetName() const {
    // 根据当前使用的王国卡判断套装名称
    std::vector<std::string> currentKingdomCards;
    for (const auto& [cardName, pile] : supply) {
        if (cardName != "Copper" && cardName != "Silver" && cardName != "Gold" &&
            cardName != "Estate" && cardName != "Duchy" && cardName != "Province" &&
            cardName != "Curse") {
            currentKingdomCards.push_back(cardName);
        }
    }
    
    // 检查是否匹配预设套装
    if (currentKingdomCards == std::vector<std::string>{
        "Village", "Cellar", "Moat", "Woodcutter", "Workshop",
        "Militia", "Market", "Smithy", "CouncilRoom", "Laboratory"}) {
        return "初学者套装";
    }
    
    if (currentKingdomCards == std::vector<std::string>{
        "Village", "Chapel", "Witch", "Gardens", "Laboratory",
        "Market", "Smithy", "Militia", "Moneylender", "Thief"}) {
        return "进阶套装";
    }
    
    return "自定义套装";
} 