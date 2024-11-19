#include "Game.h"
#include "AllCards.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include "Utils.h"

Game::Game(int numPlayers) : currentPlayer(0), turnCount(1) {
    if (numPlayers > 0) {  // 只有在创建新游戏时才初始化玩家
        for (int i = 0; i < numPlayers; i++) {
            std::string playerName;
            std::cout << "请输入玩家 " << (i + 1) << " 的名字: ";
            std::getline(std::cin, playerName);
            players.emplace_back(playerName);
        }
        initialize();
    }
}

void Game::initialize() {
    // 初始化基础卡供应堆
    const std::vector<std::pair<std::string, int>> basicCards = {
        {"Copper", 60},    // 铜币 - 60张
        {"Silver", 40},    // 银币 - 40张
        {"Gold", 30},      // 金币 - 30张
        {"Estate", players.size() <= 2 ? 8 : 24},  // 庄园 - 2人8张，3-4人24张
        {"Duchy", players.size() <= 2 ? 8 : 12},   // 公爵领地 - 2人8张，3-4人12张
        {"Province", players.size() <= 2 ? 8 : 12}, // 行省 - 2人8张，3-4人12张
        {"Curse", players.size() == 2 ? 10 : (players.size() == 3 ? 20 : 30)} // 诅咒卡
    };

    // 初始化所有供应堆
    for (const auto& [cardName, count] : basicCards) {
        supply[cardName] = std::vector<std::shared_ptr<Card>>();
        for (int i = 0; i < count; i++) {
            supply[cardName].push_back(Card::createCard(cardName));
        }
    }
    
    // 初始化10种行动卡，每种10张
    const std::vector<std::string> actionCards = {
        "Village",     // 村庄
        "Woodcutter", // 木工
        "Militia",    // 民兵
        "Market",     // 市场
        "Smithy",     // 铁匠
        "CouncilRoom",// 议会厅
        "Moneylender",// 钱庄
        "Moat",       // 护城河
        "Workshop",   // 工场
        "Cellar"      // 地窖
    };
    
    for (const auto& cardName : actionCards) {
        supply[cardName] = std::vector<std::shared_ptr<Card>>();
        for (int i = 0; i < 10; i++) {
            supply[cardName].push_back(Card::createCard(cardName));
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

    // 记录初始化信息
    std::cout << "\n=== 游戏初始化完成 ===\n";
    std::cout << "玩家数量: " << players.size() << "\n";
    std::cout << "供应堆状态:\n";
    for (const auto& [cardName, pile] : supply) {
        std::cout << cardName << ": " << pile.size() << " 张\n";
    }
    std::cout << "========================\n\n";
}

void Game::playGame() {
    while (!isGameOver()) {
        playTurn();
        
        // 询问是否保存游戏
        std::cout << "\n输入 'save' 保存游戏，'stats' 查看统计信息，或按回车继续: ";
        std::string command;
        std::getline(std::cin, command);
        
        if (Utils::equalIgnoreCase(command, "save")) {
            std::cout << "请输入存档文件名 (直接回车使用默认名称 'dominion_save.txt'): ";
            std::string filename;
            std::getline(std::cin, filename);
            
            if (filename.empty()) {
                filename = "dominion_save.txt";
            } else if (filename.find(".txt") == std::string::npos) {
                filename += ".txt";  // 自动添加.txt后缀
            }
            
            // 检查文件是否已存在
            std::ifstream checkFile(filename);
            if (checkFile.good()) {
                checkFile.close();
                std::cout << "文件 '" << filename << "' 已存在，是否覆盖? (y/n): ";
                std::string response;
                std::getline(std::cin, response);
                
                if (!Utils::equalIgnoreCase(response, "y") && 
                    !Utils::equalIgnoreCase(response, "yes")) {
                    std::cout << "取消保存\n";
                    continue;
                }
            }
            
            saveGame(filename);
        } 
        else if (Utils::equalIgnoreCase(command, "stats")) {
            displayStats();
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
                 << " 的行动点数: " << currentPlayerRef.getActions() << "\n";
        
        if (!currentPlayerRef.hasActionCard()) {
            std::cout << "没有行动卡可以使用\n";
            break;
        }
        
        std::cout << "[回合 " << turnCount << "] " 
                 << currentPlayerRef.getName() 
                 << " 选择要使用的行动卡 (输入卡片名称，或输入 'skip' 跳过): ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (Utils::equalIgnoreCase(input, "skip")) break;
        
        if (currentPlayerRef.playAction(input, *this)) {
            currentPlayerRef.useAction();
        }
    }
}

void Game::playBuyPhase() {
    Player& currentPlayerRef = players[currentPlayer];
    
    std::cout << "\n[回合 " << turnCount << " - 购买阶段]\n";
    currentPlayerRef.playAllTreasures();
    
    while (currentPlayerRef.getBuys() > 0) {

        
        // 显示可购买的卡片
        std::cout << "可购买的卡片:\n";
        std::cout << std::string(40, '-') << "\n";
        std::cout << std::setw(15) << "卡片名称" 
                 << std::setw(10) << "花费" 
                 << std::setw(10) << "剩余数量" << "\n";
        std::cout << std::string(40, '-') << "\n";
        
        for (const auto& [cardName, pile] : supply) {
            if (!pile.empty()) {
                std::cout << std::setw(15) << cardName 
                         << std::setw(10) << pile[0]->getCost()
                         << std::setw(10) << pile.size() << "\n";
            }
        }
        std::cout << std::string(40, '-') << "\n";


        int availableCoins = currentPlayerRef.getCoins();
        std::cout << "\n[" << currentPlayerRef.getName() << "] 可用金币: " 
                 << availableCoins << "\n";
        std::cout << "[" << currentPlayerRef.getName() << "] 剩余购买次数: " 
                 << currentPlayerRef.getBuys() << "\n\n";

        
        std::cout << "\n[" <<  turnCount << "-" << currentPlayerRef.getName() 
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
    // 检查省份牌是否耗尽
    if (supply.at("Province").empty()) return true;
    
    // 检查是否有三个供应堆耗尽
    int emptyPiles = 0;
    for (const auto& [cardName, pile] : supply) {
        if (pile.empty()) emptyPiles++;
    }
    return emptyPiles >= 3;
}

void Game::saveGame(const std::string& filename) {
    std::cout << "\n[回合 " << turnCount << "] 正在保存游戏到 '" << filename << "'...\n";
    
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cout << "错误: 无法创建存档文件 '" << filename << "'!\n";
        return;
    }
    
    try {
        // 保存游戏基本信息
        out.write(reinterpret_cast<const char*>(&turnCount), sizeof(turnCount));
        out.write(reinterpret_cast<const char*>(&currentPlayer), sizeof(currentPlayer));
        
        // 保存玩家数量和玩家信息
        int playerCount = players.size();
        out.write(reinterpret_cast<const char*>(&playerCount), sizeof(playerCount));
        for (const auto& player : players) {
            player.saveState(out);
        }
        
        // 保存供应堆状态
        int supplyCount = supply.size();
        out.write(reinterpret_cast<const char*>(&supplyCount), sizeof(supplyCount));
        for (const auto& [cardName, pile] : supply) {
            // 保存卡片名称
            size_t nameLength = cardName.length();
            out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
            out.write(cardName.c_str(), nameLength);
            
            // 保存牌堆大小
            int pileSize = pile.size();
            out.write(reinterpret_cast<const char*>(&pileSize), sizeof(pileSize));
        }
        
        // 同时保存日志文件
        std::string logFilename = filename;
        size_t txtPos = logFilename.find(".txt");
        if (txtPos != std::string::npos) {
            logFilename.replace(txtPos, 4, "_log.txt");
        } else {
            logFilename += "_log.txt";
        }
        logger.saveToFile(logFilename);
        
        std::cout << "游戏已成功保存到 '" << filename << "'\n";
        std::cout << "游戏日志已保存到 '" << logFilename << "'\n";
        
        // 显示存档信息摘要
        std::cout << "\n存档信息摘要:\n";
        std::cout << "- 存档文件: " << filename << "\n";
        std::cout << "- 日志文件: " << logFilename << "\n";
        std::cout << "- 当前回合: " << turnCount << "\n";
        std::cout << "- 当前玩家: " << players[currentPlayer].getName() << "\n";
        std::cout << "- 玩家列表:\n";
        for (size_t i = 0; i < players.size(); i++) {
            std::cout << "  " << (i + 1) << ". " << players[i].getName();
            if (i == currentPlayer) {
                std::cout << " (当前玩家)";
            }
            std::cout << "\n";
        }
        std::cout << "- 供应堆数量: " << supply.size() << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "保存游戏时发生错误: " << e.what() << "\n";
    }
}

void Game::loadGame(const std::string& filename) {
    std::cout << "\n正在加载游戏...\n";
    
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        throw std::runtime_error("无法打开存档文件!");
    }
    
    try {
        // 读取游戏基本信息
        in.read(reinterpret_cast<char*>(&turnCount), sizeof(turnCount));
        in.read(reinterpret_cast<char*>(&currentPlayer), sizeof(currentPlayer));
        
        // 读取玩家信息
        int playerCount;
        in.read(reinterpret_cast<char*>(&playerCount), sizeof(playerCount));
        
        players.clear();
        for (int i = 0; i < playerCount; i++) {
            Player player("");
            player.loadState(in);
            players.push_back(player);
        }
        
        // 读取供应堆状态
        int supplyCount;
        in.read(reinterpret_cast<char*>(&supplyCount), sizeof(supplyCount));
        
        supply.clear();
        for (int i = 0; i < supplyCount; i++) {
            // 读取卡片名称
            size_t nameLength;
            in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
            std::string cardName(nameLength, '\0');
            in.read(&cardName[0], nameLength);
            
            // 读取牌堆大小
            int pileSize;
            in.read(reinterpret_cast<char*>(&pileSize), sizeof(pileSize));
            
            // 重建牌堆
            std::vector<std::shared_ptr<Card>> pile;
            for (int j = 0; j < pileSize; j++) {
                pile.push_back(Card::createCard(cardName));
            }
            supply[cardName] = pile;
        }
        
        // 尝试加载游戏日志
        try {
            logger.loadFromFile(filename + ".log");
        } catch (const std::exception& e) {
            std::cout << "警告: 无法加载游戏日志 - " << e.what() << "\n";
            std::cout << "将开始新的日志记录\n";
        }
        
        // 初始化供应堆
        initialize();
        
        // 显示加载信息摘要
        std::cout << "\n游戏加载成功!\n";
        std::cout << "\n加载信息摘要:\n";
        std::cout << "- 当前回合: " << turnCount << "\n";
        std::cout << "- 玩家列表:\n";
        for (size_t i = 0; i < players.size(); i++) {
            std::cout << "  " << (i + 1) << ". " << players[i].getName();
            if (i == currentPlayer) {
                std::cout << " (当前玩家)";
            }
            std::cout << "\n";
        }
        std::cout << "- 供应堆数量: " << supply.size() << "\n";
        
        std::cout << "\n游戏将从回合 " << turnCount << " - " 
                 << players[currentPlayer].getName() << " 继续\n";
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("加载游戏失败: ") + e.what());
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
    std::cout << "\n=== 游戏结束 ===\n\n";
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

bool Game::isProvinceEmpty() const {
    auto it = supply.find("Province");
    return it == supply.end() || it->second.empty();
}

int Game::getEmptyKingdomPiles() const {
    int emptyPiles = 0;
    for (const auto& [cardName, pile] : supply) {
        // 跳过基本卡
        if (cardName == "Copper" || cardName == "Silver" || cardName == "Gold" ||
            cardName == "Estate" || cardName == "Duchy" || cardName == "Province") {
            continue;
        }
        if (pile.empty()) {
            emptyPiles++;
        }
    }
    return emptyPiles;
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
    // 首先显示基础财宝卡
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
        logger.logAction(player.getName(), "获得了 " + it->first);
        std::cout << "成功获得 " << it->first << "!\n";
    } else {
        std::cout << "无法获得该卡片\n";
    }
}

void Game::logAction(const std::string& playerName, const std::string& action) {
    logger.logAction(playerName, action);
} 