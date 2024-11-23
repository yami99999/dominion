#include "Game.h" 
#include "AllCards.h"  // 引入所有卡牌的定义。
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip> //  输入输出格式控制库。
#include "Utils.h"
#include <nlohmann/json.hpp> // 引入 nlohmann/json 库，用于处理 JSON 数据。

using json = nlohmann::json; // 为 nlohmann::json 命名空间创建别名 json，方便后续使用。

// 构造函数
    // 将当前玩家索引初始化为 0，即第一个玩家
    // 将回合数初始化为 1。
Game::Game(int numPlayers) : currentPlayer(0), turnCount(1) {
    
    // 只有在 numPlayers 大于 0 时，才初始化玩家。
    // 这意味着传入 0 表示创建一个临时的游戏实例（通常用于加载存档）
    if (numPlayers > 0) { 

        // 遍历 numPlayers 次，询问每个玩家的名字
        // 使用玩家名字构造一个 Player 对象，并添加到 players 向量中
        for (int i = 0; i < numPlayers; i++) {
            std::string playerName;
            std::cout << "Saisis le player - " << (i + 1) << " de nom: ";
            std::getline(std::cin, playerName);
            players.emplace_back(playerName); 
        }
        
        // 初始化游戏的供应堆和玩家的起始牌组
        initialize(); 
    }
}


// 初始化游戏的供应堆和玩家的起始牌组
void Game::initialize() {
    // 1.计算胜利点数卡的供应堆数量（这些数量不包括玩家初始牌组）
        // 如果玩家数量不超过2人，供应堆中的每种胜利点数卡（如庄园、公爵领地、行省）各8张。
        // 否则，3-4人游戏时，每种胜利点数卡各12张。
    int victoryCardCount = players.size() <= 2 ? 8 : 12;
    
    // 2.初始化基础卡牌供应堆
        // 定义一个包含基础卡牌名称和数量的键值对
            // - 铜币、银币、金币
            // - 庄园、公爵领地、行省
            // - 诅咒卡
    const std::vector<std::pair<std::string, int>> basicCards = {
        {"Copper", 60},    // 铜币 - 60张
        {"Silver", 40},    // 银币 - 40张
        {"Gold", 30},      // 金币 - 30张

        // 根据人数计算卡牌数量
        {"Estate", victoryCardCount},      // 庄园 - 2人8张，3-4人12张（供应堆）
        {"Duchy", victoryCardCount},       // 公爵领地 - 2人8张，3-4人12张
        {"Province", victoryCardCount},    // 行省 - 2人8张，3-4人12张
        {"Curse", players.size() == 2 ? 10 : 
                 (players.size() == 3 ? 20 : 30)}    // 诅咒卡
    };


    // 3.遍历每种基础卡牌，初始化供应堆
    for (const auto& [cardName, count] : basicCards) {
        
        // 为每种卡牌名称创建一个新的卡牌堆
        supply[cardName] = std::vector<std::shared_ptr<Card>>();

        // 创建指定数量的卡牌对象，并添加到供应堆中
        for (int i = 0; i < count; i++) {

            // 使用 Card::createCard 工厂方法创建卡牌对象，并将其添加到供应堆。
            supply[cardName].push_back(Card::createCard(cardName));
        }
    }
    


    // 4.初始化10种行动卡，每种10张
        
    // 定义一个包含所有行动卡牌名称的向量
    const std::vector<std::string> actionCards = {
        "Village", "Woodcutter", "Militia", "Market", "Smithy",
        "CouncilRoom", "Moneylender", "Moat", "Workshop", "Cellar"
    };
    
    
    // 遍历每种行动卡牌，初始化供应堆。
    for (const auto& cardName : actionCards) {
        supply[cardName] = std::vector<std::shared_ptr<Card>>();
       
        // 每种行动卡牌各10张。
        for (int i = 0; i < 10; i++) {
            supply[cardName].push_back(Card::createCard(cardName));
        }
    }
    



    // 5.为每个玩家初始化牌组（使用额外的Estate卡，不从供应堆中取）
    
    // 遍历所有玩家，初始化他们的牌组
    for (auto& player : players) {
        
        // 7张铜币（从供应堆中取）
        // 从供应堆中取出7张铜币并添加到玩家的弃牌堆中
        for (int i = 0; i < 7; i++) {
            
            // 检查卡牌是否足够, 确保不能从空的卡牌堆中取卡
            if (!supply["Copper"].empty()) {

                // 执行分发操作
                    // supply["Copper"].back() => 取出 supply["Copper"] 的最后一张卡牌。
                    // player.addToDiscard(...) => 将卡牌添加到玩家的弃牌区
                player.addToDiscard(supply["Copper"].back());

                    // 从 supply["Copper"] 中移除这张卡牌，防止重复使用。
                supply["Copper"].pop_back();
            }
        }
        // 3张庄园（创建新的，不从供应堆中取）
        for (int i = 0; i < 3; i++) {
            player.addToDiscard(Card::createCard("Estate"));
        }
        player.shuffleDeck(); // 洗牌玩家的牌组
        player.drawCard(5); // 抽取起始手牌
    }






    // 6. 显示初始化信息
    std::cout << "\n=== Initialisation du jeu terminée ===\n";
    std::cout << "Nombre de joueurs: " << players.size() << "\n";
    std::cout << "Nombre de cartes de points de victoire dans la pile de réserve: " << victoryCardCount << " cartes par type\n";
    std::cout << "l'état de réserve:\n";
    displaySupplyPiles();
    std::cout << "========================\n\n";
}



// 游戏进程
void Game::playGame() {

    // 主游戏循环
    while (!isGameOver()) {
        playTurn(); // 1.执行当前玩家的回合
        
        // 2.显示当前供应堆状态
        int emptyPiles = getEmptyPiles(); // 获取当前空的供应堆数量

        // 3.如果有空的供应堆，输出每个供应堆的状态，包括已耗尽和剩余数量
        if (emptyPiles > 0) {
            std::cout << "\n当前供应堆状态:\n";
            for (const auto& [cardName, pile] : supply) {
                if (pile.empty()) {
                    std::cout << "- " << cardName << " 堆已耗尽\n";
                } else {
                    std::cout << "- " << cardName << ": 剩余 " 
                             << pile.size() << " 张\n";
                }
            }
            std::cout << "总计: " << emptyPiles << " 个供应堆已耗尽\n";
        }
        
        // 4.询问是否保存游戏
        std::cout << "\nEntrez 'save' pour sauvegarder la partie, 'stats' pour voir les statistiques, ou appuyez sur Entrée pour continuer: ";
        std::string command;
        std::getline(std::cin, command);
        
        // 5.如果用户输入 "save"（不区分大小写），执行保存游戏的流程。
        if (Utils::equalIgnoreCase(command, "save")) {
            std::cout << "Veuillez entrer le nom du fichier de sauvegarde (appuyez sur Entrée pour utiliser le nom par défaut 'dominion_save.txt') :\n ";
            std::string filename;
            std::getline(std::cin, filename);
            
            if (filename.empty()) {
                filename = "dominion_save.txt";
            } 
            else if (filename.find(".txt") == std::string::npos) {
                filename += ".txt";  // 自动添加.txt后缀
            }
            
            // 检查文件是否已存在
            std::ifstream checkFile(filename);
            if (checkFile.good()) {
                checkFile.close();
                std::cout << "Le fichier '" << filename << "' existe déjà, voulez-vous l'écraser ?  (y/n): ";
                std::string response;
                std::getline(std::cin, response);
                
                if (!Utils::equalIgnoreCase(response, "y") && 
                    !Utils::equalIgnoreCase(response, "yes")) {
                    std::cout << "Annulation de la sauvegarde.\n";
                    continue;
                }
            }
            
            // 将游戏状态保存到指定文件
            saveGame(filename);
        } 
        // 6.如果用户输入 "stats"，调用 displayStats 方法，显示游戏统计信息
        else if (Utils::equalIgnoreCase(command, "stats")) {
            displayStats();
        }
    }
    
    // 游戏结束处理: 显示最终状态和得分
    displayFinalState(); // 显示游戏结束时的最终状态。
    auto scores = calculateFinalScores(); // 计算所有玩家的最终得分。
    displayResults(scores);  // 显示游戏结束后的得分结果
}

// 回合函数
void Game::playTurn() {
    // 1.获取当前玩家的引用
    Player& currentPlayerRef = players[currentPlayer];
    
    // 2.显示当前回合信息
    std::cout << "\n==========  tour: " << turnCount << " - " 
              << currentPlayerRef.getName() << " ==========\n";
    
    // 3.显示牌堆状态
    currentPlayerRef.showDeckStatus();
    
    // 4. 显示当前手牌: [当前玩家] + 手牌
    std::cout << "[" << currentPlayerRef.getName() << "] ";
    currentPlayerRef.showHand();
    
    // 6.行动阶段(包括使用行动卡)
    playActionPhase();
    
    // 7.购买阶段(购买卡牌)
    playBuyPhase();
    
    // 8.清理阶段(弃牌和抽取新手牌)
    playCleanupPhase();
    
    // 9.切换到下一个玩家
    // 使用取模运算确保循环到第一个玩家
    currentPlayer = (currentPlayer + 1) % players.size();
    
    // 10.如果回到第一个玩家，回合数加1
    if (currentPlayer == 0) {
        turnCount++;
    }
}





/////////////////////////////////////////////////////////////////////////////////////





// 1.行动阶段
void Game::playActionPhase() {
    // 1.获取当前玩家的引用
    Player& currentPlayerRef = players[currentPlayer];
    

    // 2.显示行动阶段提示: 当前回合数 + 行动阶段
    std::cout << "\n[tour: " << turnCount << " - Phase d'action ]\n";
    

    // 3. 行动阶段循环: 直到当前玩家没有剩余的行动点数
    while (currentPlayerRef.getActions() > 0) {

        // 显示当前行动点数
        std::cout << "[tour " << turnCount << "] " 
                 << currentPlayerRef.getName() 
                 << " point d'action: " << currentPlayerRef.getActions() << "\n";
        
        // 检查是否有可用的行动卡
            // 如果玩家手牌中没有行动卡，则输出提示并跳出循环
        if (!currentPlayerRef.hasActionCard()) {
            std::cout << "pas de cartes d'action disponibles\n";
            break;
        }
        
        // 提示玩家选择行动卡
        std::cout << "[tour " << turnCount << "] " 
                 << currentPlayerRef.getName() 
                 << " Sélectionne la carte d'action à utiliser (saisis le nom de la carte, 'info card name' pour la description, 'skip' pour passer): ";
        // 读取输入
        std::string input;
        std::getline(std::cin, input);
        

        // 处理玩家选择
            // 跳过行动卡的使用
        if (Utils::equalIgnoreCase(input, "skip")) break;
        
            // 检查是否是查看卡片信息的命令
            // 如果输入以 "info" 开头，提取卡片名称并显示卡片描述。
        if (input.substr(0, 4) == "info") {
            std::string cardName = input.substr(5);
            currentPlayerRef.showCardDescription(cardName);
            continue;
        }
        

            // 尝试使用行动卡 
        if (currentPlayerRef.playAction(input, *this)) {// *this 指代游戏状态
            // 如果成功，调用 useAction 减少玩家的行动点数。
            currentPlayerRef.useAction();
        }
    }
}


// 2.购买阶段
void Game::playBuyPhase() {
    // 获取当前玩家的引用
    Player& currentPlayerRef = players[currentPlayer];
    
    // 显示购买阶段提示: 当前玩家的购买阶段
    std::cout << "\n=== " << currentPlayerRef.getName() << " - Phase d'achete ===\n";
    
    // 自动使用所有财富卡 = 显示手牌的总金额
    currentPlayerRef.playAllTreasures();
    


    // 购买循环: 直到玩家没有剩余的购买次数
    while (currentPlayerRef.getBuys() > 0) {

        // 显示可用金币和剩余购买次数
        int availableCoins = currentPlayerRef.getCoins();
        std::cout << "\n[" << currentPlayerRef.getName() << "] Pièces disponibles: " 
                 << availableCoins << "\n";
        std::cout << "[" << currentPlayerRef.getName() << "] Achats restants: " 
                 << currentPlayerRef.getBuys() << "\n";
        
        // 使用新的显示方法 - 显示当前供应堆状态
        displaySupplyPiles();
        

        // 提示玩家购买
        std::cout << "\n[" << currentPlayerRef.getName() 
                 << "] Saisis le nom de la carte à acheter, ou tape 'done' pour terminer la phase d'achat:  \n";
        
        
        // 玩家输入
        std::string input;
        std::getline(std::cin, input);
        

        // 处理玩家输入
            // 结束购买阶段
        if (Utils::equalIgnoreCase(input, "done")) break;
        
            // 检查是否是查看卡片信息的命令
        if (input.substr(0, 4) == "info") {
            std::string cardName = input.substr(5);
            showSupplyCardDescription(cardName);
            continue;
        }
            // 查找卡片:在供应堆中查找与用户输入名称匹配的卡片
        auto it = std::find_if(supply.begin(), supply.end(),
            [&input](const auto& pair) {
                return Utils::equalIgnoreCase(pair.first, input);
            });
        

            // 1.检查卡片是否存在且未售罄

            // 1.如果找到卡片且供应堆中有剩余，则继续处理购买逻辑
        if (it != supply.end() && !it->second.empty()) {
            int cardCost = it->second[0]->getCost(); // 获取卡片花费
            
            // 2.检查金币是否足够
                // 2.如果足够, 则进行购买
            if (cardCost <= availableCoins) { 
                // 购买卡片
                std::shared_ptr<Card> boughtCard = it->second.back(); // 获取供应堆中的最后一张卡片（假设堆顶是末尾）
                it->second.pop_back(); // 从供应堆中移除购买的卡片
                currentPlayerRef.addToDiscard(boughtCard); // 将购买的卡片添加到玩家的弃牌堆中
                currentPlayerRef.spendCoins(cardCost); // 扣除玩家的金币
                currentPlayerRef.useBuy(); // 扣除玩家的购买次数
                
                // 记录日志
                logger.recordCardBought(it->first); // 记录购买的卡片类型
                
                // 记录玩家的购买动作
                logger.logAction(currentPlayerRef.getName(), 
                    "acheté:  " + it->first + " (dépenser: " + std::to_string(cardCost) + " pièces)");
                


                // 输出购买成功信息
                std::cout << "Achat réussi " << it->first << "!\n"; // 成功购买
                

                // 检查是否需要提前结束购买阶段
                    // 如果玩家的购买次数已用完，输出提示并跳出循环。
                    // 如果玩家的金币已用完，输出提示并跳出循环
                if (currentPlayerRef.getBuys() == 0) {
                    std::cout << "Il ne reste plus d'achats.\n"; // 没有剩余购买次数了
                    break;
                }
                if (currentPlayerRef.getCoins() == 0) {
                    std::cout << "Il ne reste plus de pièces.\n"; // 没有剩余金币了
                    break;
                }
            } 
            
            // 2.若金币不够
            else {
                std::cout << "Pièces insuffisantes ! Il vous faut " << cardCost << " pièces\n";
            }
        } 
        
        
        // 1.若卡片不存在或已售罄
        else {
            if (it == supply.end()) {
                std::cout << "Ne trouve pas la carte.\n";
            } 
            else {
                std::cout << "Cette carte est vendue\n";
            }
        }
    }
}



// 3.结束阶段
void Game::playCleanupPhase() {
    // 获取当前玩家的引用
    Player& currentPlayerRef = players[currentPlayer];

    // 清理阶段操作: 
    currentPlayerRef.discardHand(); //1.将当前玩家的手牌弃置到弃牌堆中。
    currentPlayerRef.drawCard(5); //2.当前玩家抽取5张牌作为新的手牌。

    // 重置玩家状态
    currentPlayerRef.resetActions(); // 3.将行动点数重置为1
    currentPlayerRef.resetBuys(); //4.将购买次数重置为1
    currentPlayerRef.resetCoins(); //5.金币数重置为0
}



/////////////////////////////////////////////////////



// 1.检查游戏结束条件
bool Game::isGameOver() const {
    // 如果满足任一结束条件，返回 true，表示游戏结束
    // 1. 检查Province堆是否为空
    if (isProvinceEmpty()) {
        std::cout << "\n*** Fin de la partie : La pile de Provinces est épuisée! ***\n";
        return true;
    }
    
    // 2. 检查是否有任意三堆卡牌堆空（括所有类型的卡）
    int emptyPiles = getEmptyPiles();
    if (emptyPiles >= 3) {
        std::cout << "\n*** Fin de la partie : " << emptyPiles << " piles de la réserve sont épuisées ! ***\n";
        return true;
    }
    
    // 否则，返回 false，继续游戏
    return false;
}


// 1.1检查“行省”堆是否为空:
bool Game::isProvinceEmpty() const {
    auto it = supply.find("Province");
    return it == supply.end() || it->second.empty();
}


// 1.2计算空的供应堆数量
int Game::getEmptyPiles() const {
    int emptyPiles = 0; // 初始化空堆计数器。
    
    // 检查所有供应堆，包括基础卡和王国卡
    for (const auto& [cardName, pile] : supply) {
        if (pile.empty()) {
            emptyPiles++;
        }
    }
    
    return emptyPiles;
}








// 保存游戏状态到文件
void Game::saveGame(const std::string& filename) {


    // 输出保存开始的信息
    std::cout << "\n[tour: " << turnCount << "] Sauvegarde le jeu...\n";
    
    // 使用 try-catch 块捕捉可能发生的异常（如文件写入错误）
    try {
        json saveData; // 创建一个 JSON 对象，用于存储所有游戏状态数据
        
        // 保存游戏基本信息
        saveData["turn_count"] = turnCount;
        saveData["current_player"] = currentPlayer;
        
        // 保存玩家信息
        json playersData = json::array(); // 创建一个 JSON 数组，用于存储所有玩家的信息。
        // 遍历每个玩家，保存他们的状态
        for (const auto& player : players) {
            
            // 将单个玩家的数据添加到玩家数组中
            playersData.push_back(player.toJson());
        }
        // 将所有玩家的数据添加到主 JSON 对象中。
        saveData["players"] = playersData;
        



        // 保存供应堆状态
        json supplyJson;// 创建一个 JSON 对象，用于存储供应堆的状态
        // 遍历所有供应堆，记录每种卡牌剩余的数量。
        for (const auto& [cardName, pile] : supply) {
            json pileJson = json::array();
            for (const auto& card : pile) {
                pileJson.push_back(card->toJson());
            }
            supplyJson[cardName] = pileJson;
        }
        saveData["supply"] = supplyJson; // 将供应堆数据添加到主 JSON 对象中
        
        // 保存游戏日志
        saveData["game_log"] = logger.getJson(); // 获取游戏日志的 JSON 表示，并添加到主 JSON 对象中
        
        // 写入文件
        std::ofstream out(filename);
        if (!out) { // 如果文件无法打开，抛出异常。
            throw std::runtime_error("Impossible de créer un fichier d'archive !");
        }

        // 将 JSON 对象格式化输出到文件中，使用缩进提高可读性。
        out << std::setw(4) << saveData << std::endl;
        
        // 输出保存成功信息
        std::cout << "Le jeu a été sauvegardé avec succès dans " << filename << "\n";
        
        // 显示存档信息摘要
        std::cout << "\nRésumé de l'information archivée: \n";
        std::cout << "- Tour actuel: " << turnCount << "\n";
        std::cout << "- Player actuel: " << players[currentPlayer].getName() << "\n";
        std::cout << "- Nombre de players: " << players.size() << "\n";
        std::cout << "- Nombre de piles de réserve: " << supply.size() << "\n";
        
    } catch (const std::exception& e) { // 异常处理:如果在保存过程中发生异常，捕捉并输出错误信息。
        std::cout << "Une erreur s'est produite lors de l'enregistrement du jeu: " << e.what() << "\n";
    }
}




// 加载游戏状态从文件
void Game::loadGame(const std::string& filename) {
    std::cout << "\nChargement du jeu...\n";
    
    // 使用 try-catch 块捕捉可能发生的异常（如文件读取错误、JSON 解析错误）
    try {

        // 打开文件
        std::ifstream in(filename); // 打开指定的文件进行读取
        if (!in) { // 如果文件无法打开，抛出异常。
            throw std::runtime_error("Impossible d'ouvrir le fichier d'archive !");
        }
        

        // 解析 JSON 数据
        json saveData = json::parse(in); // 使用 nlohmann::json 库解析文件内容到 saveData 对象中。
        
        // 加载游戏基本信息
        turnCount = saveData["turn_count"];
        currentPlayer = saveData["current_player"];
        
        // 加载玩家信息
        players.clear(); // 清空当前的玩家列表，以便重新加载。

        // 遍历保存的每个玩家的数据
        for (const auto& playerJson : saveData["players"]) {
            Player player("");  // 创建临时玩家
            player.fromJson(playerJson);
            players.push_back(player);
        }
        
        // 加载供应堆状态
        supply.clear(); // 清空当前的供应堆，以便重新加载。

        // 遍历保存的每种卡牌的供应堆信息。
        for (const auto& [cardName, pileJson] : saveData["supply"].items()) {
                    std::vector<std::shared_ptr<Card>> pile;
                    for (const auto& cardJson : pileJson) {
                        pile.push_back(Card::fromJson(cardJson));
                    }
                    supply[cardName] = pile;
                }
        
        // 加载游戏日志
        if (saveData.contains("game_log")) {
            logger.loadFromJson(saveData["game_log"]);
        }
        

        // 输出加载成功的信息
        std::cout << "Le jeu est sorti de " << filename ;
        std::cout << "Tour actuel: " << turnCount << "\n";
        std::cout << "Player actuel: " << players[currentPlayer].getName() << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "Une erreur s'est produite lors du chargement du jeu: " << e.what() << "\n";
        std::cout << "Réinitialise le jeu...\n";
        initialize(); // 如果加载失败，重新初始化游戏，开始一个新的游戏。
    }
}







////////////////////////////////////////////////////////////////////////////////////









// 计算所有玩家的最终得分
std::vector<std::pair<std::string, int>> Game::calculateFinalScores() const {
    
    // 创建一个向量，用于存储每个玩家的姓名和得分
    std::vector<std::pair<std::string, int>> scores;
    
    // 遍历所有玩家。
    for (const auto& player : players) {
        int totalScore = 0; // 初始化当前玩家的总得分。

        // 遍历玩家所有的卡牌（牌组、手牌、弃牌堆），累加每张卡牌的胜利点数。
        for (const auto& card : player.getAllCards()) {
            totalScore += card->getVictoryPoints(); 
        }
        scores.emplace_back(player.getName(), totalScore); // 将玩家的姓名和总得分添加到 scores 向量中。
    }
    

    // 使用自定义的比较函数，将 scores 向量按得分从高到低排序。
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return scores; // 返回排序后的得分列表。
}






// 显示游戏结束后的得分结果
void Game::displayResults(const std::vector<std::pair<std::string, int>>& scores) {
    std::cout << "\n=== La partie est terminée. ===\n\n";
    std::cout << "Score final:\n";
    

    // 获取最高得分，用于标记获胜者
    int highestScore = scores[0].second; 

    // 遍历所有玩家的得分。
    for (const auto& [name, score] : scores) {

        // 输出玩家的姓名和得分。
        std::cout << name << ": " << score << " 点";
        if (score == highestScore) { // 如果玩家的得分等于最高得分，添加一个奖杯符号，表示获胜者。
            std::cout << " 🏆";
        }
        std::cout << "\n";
    }
}




// 显示游戏结束时的最终状态
void Game::displayFinalState() const {
    
    std::cout << "\n=== L'état de fin ===\n";
    std::cout << "Nombre total de tours: " << turnCount << "\n"; // 总回合数
    
    
    // 空的供应堆名称输出
    std::cout << "Piles de résevre vides:\n";
    for (const auto& [cardName, pile] : supply) {
        if (pile.empty()) {
            std::cout << "- " << cardName << "\n";
        }
    }
    std::cout << "==================\n\n";
}




// 显示供应堆状态
void Game::displayStats() {
    std::cout << "\n=== Statistiques du jeu ===\n"; // 游戏统计信息
    std::cout << "L'état de réserve:\n"; // 供应堆状态
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte"   // 卡片名称
              << std::setw(10) << "Quantité restante" << "\n";  // 剩余数量
    std::cout << std::string(40, '-') << "\n";
    


    // 按类型分组显示
    
    // 首先示基础财宝卡
    for (const auto& [cardName, pile] : supply) {
        // 如果卡牌名称是 "Estate"、"Duchy"、"Province" 或 "Curse"，则显示其剩余数量。
        if (cardName == "Copper" || cardName == "Silver" || cardName == "Gold") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    
    // 然后显示胜利点数卡和诅咒卡
    for (const auto& [cardName, pile] : supply) {
        // 如果卡牌名称是 "Estate"、"Duchy"、"Province" 或 "Curse"，则显示其剩余数量。
        if (cardName == "Estate" || cardName == "Duchy" || 
            cardName == "Province" || cardName == "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    
    // 最后显示王国卡
    for (const auto& [cardName, pile] : supply) {
        // 如果卡牌名称不是上述任何一种，则认为是王国卡，并显示其剩余数量
        if (cardName != "Copper" && cardName != "Silver" && cardName != "Gold" &&
            cardName != "Estate" && cardName != "Duchy" && 
            cardName != "Province" && cardName != "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    
    std::cout << std::string(40, '-') << "\n";
    







    // 显示每个玩家的牌堆状态
    std::cout << "\nÉtat de la pile de players: \n";

    // 遍历所有玩家，显示他们的牌堆状态
    for (const auto& player : players) {
        std::cout << player.getName() << ": ";
        player.showDeckStatus(); // 显示玩家的抽牌堆和弃牌堆的数量 
    }
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* 对其他玩家的卡牌效果函数 */



// 强制其他玩家弃牌至三张
void Game::otherPlayersDiscardToThree(const Player& currentPlayer) {
    // 遍历所有玩家
    for (auto& player : players) {
        // 排除当前玩家，处理其他玩家
        if (&player != &currentPlayer) {
            // 检查是否有护城河防御
            if (!player.hasMoat()) {
                int handSize = player.getHandSize(); // 如果玩家没有护城河卡牌，检查手牌数量。
                if (handSize > 3) {
                    std::cout << "\n" << player.getName() << " Doit être mis au défausse: " 
                             << (handSize - 3) << " nombre de carte\n";
                    // 如果手牌数量超过3张，提示并调用 discardDownTo 方法，让玩家弃牌至3张。
                    player.discardDownTo(3);
                }
            } 
            // 如果玩家有护城河卡牌，输出防御信息并记录日志。
            else {
                // 输出信息
                std::cout << player.getName() << " La carte Fossé a été utilisée pour contrer une attaque.\n"; // 使用护城河防御了攻击
                // 记录日志
                logger.logAction(player.getName(), "La carte Fossé a été utilisée pour contrer une attaque."); // 使用护城河防御了攻击
            }
        }
    }
}


// 让其他玩家抽取指定数量的牌
void Game::otherPlayersDraw(const Player& currentPlayer, int count) {
    
    // 遍历所有玩家。
    for (auto& player : players) {
        // 排除当前玩家，处理其他玩家。
        if (&player != &currentPlayer) {
            player.drawCard(count); // 调用玩家的 drawCard 方法，让其抽取 count 张牌。
            logger.logAction(player.getName(), // 记录玩家抽牌的动作。
                "Piocher " + std::to_string(count) + " cartes"); // 抽了...张牌
        }
    }
}






// 让玩家获得一张不超过指定花费的卡牌
void Game::gainCardUpToCost(Player& player, int maxCost) {

    // 显示可获得的卡片
    std::cout << "\n=== Obtenir une carte ===\n"; // 获得一张卡片
    std::cout << "Cartes disponibles (Coût maximal : " << maxCost << "):\n"; // 可获得的卡片 (最大花费:
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte |  " // 卡片名称
             << std::setw(10) << "Coût |  "  // 花费
             << std::setw(10) << "Quantité restante" << "\n"; // 剩余
    std::cout << std::string(40, '-') << "\n";
    



    // 遍历所有供应堆。
    for (const auto& [cardName, pile] : supply) {
        // 如果供应堆不为空且卡牌花费不超过 maxCost，则显示卡牌信息。
        if (!pile.empty() && pile[0]->getCost() <= maxCost) {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile[0]->getCost()
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    std::cout << std::string(40, '-') << "\n";
    


    // 提示玩家输入要获得的卡片名称
    std::cout << "\n请输入要获得的卡片名称: ";
    std::string input;
    std::getline(std::cin, input);
    

    // 查找并获取卡片
    // 在供应堆中查找与用户输入名称匹配的卡片。
    auto it = std::find_if(supply.begin(), supply.end(),
        [&input](const auto& pair) {
            return Utils::equalIgnoreCase(pair.first, input);
        });
    // 如果找到卡片且供应堆中有剩余且花费不超过 maxCost，则执行购买    
    if (it != supply.end() && !it->second.empty() && 
        it->second[0]->getCost() <= maxCost) {
        std::shared_ptr<Card> gainedCard = it->second.back(); // 获取供应堆中的最后一张卡片。
        it->second.pop_back(); // 从供应堆中移除购买的卡片。
        player.addToDiscard(gainedCard); // 将购买的卡片添加到玩家的弃牌堆中。
        
        // 记录玩家获得卡片的动作。
        logger.logAction(player.getName(), "Obtenir " + it->first); 
        std::cout << "Succès: " << it->first << "!\n";
    } 
    // 如果无法获得卡片（卡片不存在、已售罄或花费超过），输出提示信息。
    else {
        std::cout << "Impossible d'obtenir la carte\n"; // 无法获得卡片
    }
}














// 记录玩家的动作
void Game::logAction(const std::string& playerName, const std::string& action) {
    // 调用 GameLogger 的 logAction 方法，记录玩家的动作描述
    logger.logAction(playerName, action);
}





// 显示指定卡牌的描述
void Game::showSupplyCardDescription(const std::string& cardName) const {
    
    // 在供应堆中查找指定名称的卡片。
    auto it = supply.find(cardName);
    
    // 如果找到卡片且供应堆中有剩余，调用卡片的 getDescription 方法并输出描述。
    if (it != supply.end() && !it->second.empty()) {
        std::cout << "\n" << it->second[0]->getDescription() << "\n";
    }   
    // 否则，输出提示信息表示Ne trouve pas la carte.。
    else {
        std::cout << "Cette carte n'est pas disponible dans la réserve. \n"; //在供应堆中Ne trouve pas la carte.。
    }
}




// 显示所有供应堆的详细状态
void Game::displaySupplyPiles() const {
    std::cout << "\n=== État de la réserve ===\n"; // 显示所有供应堆的详细状态
    
    // 1.显示金币卡
    std::cout << "\nCartes Trésor: \n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte  |  " 
              << std::setw(10) << "Coût  |  " 
              << std::setw(10) << "Quantité restante" << "\n";
    std::cout << std::string(40, '-') << "\n";

    const std::vector<std::string> treasureCards = {"Copper", "Silver", "Gold"};
    
    // 遍历所有财宝卡牌，显示其花费和剩余数量。
    for (const auto& cardName : treasureCards) {
        // 如果财宝卡堆存在且不为空，显示其花费和剩余数量。
        if (supply.count(cardName) && !supply.at(cardName).empty()) {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << supply.at(cardName)[0]->getCost()
                     << std::setw(10) << supply.at(cardName).size() << "\n";
        }
    }
    




    // 2.显示胜利点数卡
    std::cout << "\nCartes Victoire:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte  |  " 
              << std::setw(10) << "Coût  |  " 
              << std::setw(10) << "Quantité restante" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    const std::vector<std::string> victoryCards = {"Estate", "Duchy", "Province"};
    // 遍历所有胜利点数卡牌，显示其花费和剩余数量。
    for (const auto& cardName : victoryCards) {
        // 如果胜利点数卡堆存在且不为空，显示其花费和剩余数量。
        if (supply.count(cardName) && !supply.at(cardName).empty()) {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << supply.at(cardName)[0]->getCost()
                     << std::setw(10) << supply.at(cardName).size() << "\n";
        }
    }
    






    // 3.显示诅咒卡
    std::cout << "\nCartes Malédiction:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte  |  " 
              << std::setw(10) << "Coût  |  " 
              << std::setw(10) << "Quantité restante" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    // 如果诅咒卡堆存在且不为空，显示其花费和剩余数量。
    if (supply.count("Curse") && !supply.at("Curse").empty()) {
        std::cout << std::setw(15) << "Curse"
                 << std::setw(10) << supply.at("Curse")[0]->getCost()
                 << std::setw(10) << supply.at("Curse").size() << "\n";
    }
    








    // 4.显示行动卡
    std::cout << "\nCartes Action:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte  |  " 
              << std::setw(10) << "Coût  |  " 
              << std::setw(10) << "Quantité restante" << "\n";
    // 结尾分割符
    std::cout << std::string(40, '-') << "\n";
    // 遍历所有供应堆，筛选出行动卡（不是财宝卡、胜利点数卡或诅咒卡），并显示其花费和剩余数量。
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

    // 结尾分割符
    std::cout << std::string(40, '-') << "\n";
} 