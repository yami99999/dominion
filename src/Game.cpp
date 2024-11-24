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
// Définir toutes les cartes de royaume disponibles
const std::vector<std::string> Game::ALL_KINGDOM_CARDS = {
    "Village", "Woodcutter", "Militia", "Market",
    "Smithy", "CouncilRoom", "Moneylender", "Moat",
    "Workshop", "Cellar", "Witch", "Thief",
    "Gardens", "Chapel", "Feast", "Laboratory"
};

Game::Game(int numPlayers, bool randomKingdom, const std::string& kingdomSet) 
    : currentPlayer(0), turnCount(1) {
    
    std::cout << "\n=== Créer un nouveau jeu ===\n";
    std::cout << "Nombre de joueurs: " << numPlayers << "\n";
    
    // 创建玩家 Créer les joueurs
    for (int i = 0; i < numPlayers; i++) {
        std::string playerName;
        std::cout << "Entrez le nom du joueur " << (i + 1) << " : ";
        std::getline(std::cin, playerName);
        players.emplace_back(playerName);
    }
    
    // 根据参数选择王国卡初始化方式 
    // Choisir l'initialisation des cartes de royaume en fonction des paramètres
    std::vector<std::string> selectedCards;
    if (randomKingdom) {
        selectedCards = getRandomKingdomCards();
    } else if (!kingdomSet.empty()) {
        selectedCards = getPresetKingdomCards(kingdomSet);
    } else {
        selectedCards = selectKingdomCards();
    }
    
    // 记录选择的王国卡 Enregistrer les cartes de royaume choisies
    logger.logKingdomCardSelection(selectedCards);
    
    // 初始化游戏 Initialiser le jeu
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
    
    std::cout << "Ensemble prédéfini inconnu, sélection aléatoire utilisée\n"; // 未知套装, 随机选择
    return getRandomKingdomCards();
}

void Game::initialize() { 
    // 默认初始化，使用随机选择的王国卡 
    // Initialisation par défaut, utilisation des cartes de royaume choisies au hasard
    initialize(getRandomKingdomCards());
}

std::vector<std::string> Game::selectPresetCards() {
    Utils::printHeader("Choix des ensembles prédéfinis"); // 预设套装选择
    std::cout << "Ensembles prédéfinis disponibles:\n"; // 可用的预设套装
    std::cout << "1. Ensemble débutant\n";      // 1. 初学者套装
    std::cout << "2. Ensemble avancé\n";   // 2. 进阶套装
    std::cout << "3. Ensemble aléatoire\n";   // 3. 随机套装
    
    int choice;
    std::cout << "\nSélectionnez un ensemble (1-3): ";   // 请选择套装 (1-3)
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
        // Initialisation des piles de cartes de base
        const std::vector<std::pair<std::string, int>> basicCards = {
            {"Copper", 60},    // 铜币 - 60张  Pièce de cuivre - 60 cartes
            {"Silver", 40},    // 银币 - 40张  Pièce d'argent - 40 cartes
            {"Gold", 30},      // 金币 - 30张  Pièce d'or - 30 cartes
            {"Estate", players.size() <= 2 ? 8 + players.size()*3 : 12 + players.size()*3}, // Domaine   // 庄园(!!难以修复, 所以直接加玩家数量*3) 
            {"Duchy", players.size() <= 2 ? 8 : 12},     // 公爵领地 // Duché
            {"Province", players.size() <= 2 ? 8 : 12},  // 行省 // Province
            {"Curse", players.size() == 2 ? 10 : 
                     (players.size() == 3 ? 20 : 30)}    // 诅咒卡 // Carte malédiction
        };

        // 初始化基础卡
        // Initialisation des cartes de base
        for (const auto& [cardName, count] : basicCards) {
            supply[cardName] = std::vector<std::shared_ptr<Card>>();
            for (int i = 0; i < count; i++) {
                supply[cardName].push_back(Card::createCard(cardName));
            }
        }
        
        // 初始化选中的王国卡
        // Initialisation des cartes de royaume sélectionnées
        for (const auto& cardName : selectedCards) {
            try {
                supply[cardName] = std::vector<std::shared_ptr<Card>>();
                for (int i = 0; i < 10; i++) {
                    supply[cardName].push_back(Card::createCard(cardName));
                }
            } catch (const std::exception& e) {
                std::cerr << "Échec de l'initialisation de la carte: " << cardName << " - " << e.what() << std::endl; // 初始化卡片失败:
                throw std::runtime_error("Échec de l'initialisation de la carte de royaume : " + cardName); // 初始化王国卡失败:
            } 
        }
        
        // 为每个玩家初始化牌组
        // Initialisation du deck de chaque joueur
        for (auto& player : players) {
            // 从供应堆中取出初始卡牌
            // Retirer les cartes initiales de la pile
            for (int i = 0; i < 7; i++) {  // 7张铜币   7 pièces de cuivre
                if (!supply["Copper"].empty()) {
                    player.addToDiscard(supply["Copper"].back());
                    supply["Copper"].pop_back();
                }
            }
            for (int i = 0; i < 3; i++) {  // 3张庄园 3 domaines
                if (!supply["Estate"].empty()) {
                    player.addToDiscard(supply["Estate"].back());
                    supply["Estate"].pop_back();
                }
            }
            player.shuffleDeck();
            player.drawCard(5); // 抽取起始手牌  Tirer les cartes initiales
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Échec de l'initialisation du jeu: " << e.what() << std::endl;  // 游戏初始化失败
        throw;
    }
}

void Game::playGame() {
    while (!isGameOver()) {
        playTurn();
        
        // 回合结束后的命令处理循环  
        // Boucle de gestion des commandes après chaque tour
        while (true) {
            std::cout << "\nTapez 'save' pour sauvegarder le jeu, 'stats' pour voir les statistiques, 'go' pour continuer : ";  // 输入 'save' 保存游戏，'stats' 查看统计信息，'go' 继续游戏
            std::string command;
            std::getline(std::cin, command);
            
            if (Utils::equalIgnoreCase(command, "save")) {
                std::cout << "Veuillez entrer un nom de fichier pour la sauvegarde (appuyez sur Entrée pour utiliser le nom par défaut * dominion_save.txt *: "; // 请输入存档文件名 (直接回车使用默认名称 'dominion_save.txt'):
                std::string filename;
                std::getline(std::cin, filename);
                
                if (filename.empty()) {
                    filename = "dominion_save.txt";
                } else if (filename.find(".txt") == std::string::npos) {
                    filename += ".txt";
                }
                
                // 检查文件是否已存在
                // Vérifier si le fichier existe déjà
                std::ifstream checkFile(filename);
                if (checkFile.good()) {
                    checkFile.close();
                    std::cout << "Le fichier '" << filename << "' existe déjà. Voulez-vous le remplacer ? (y/n): ";  // 是否确认保存? (y/n)
                    std::string response;
                    std::getline(std::cin, response);
                    
                    if (!Utils::equalIgnoreCase(response, "y") && 
                        !Utils::equalIgnoreCase(response, "yes")) {
                        std::cout << "Sauvegarde annulée\n"; // 取消保存
                        continue;  // 返回命令输入循环 // Retourner à la boucle des commandes
                    }
                }
                
                saveGame(filename);
                continue;  // 返回命令输入循环 // Retourner à la boucle des commandes
            } 
            else if (Utils::equalIgnoreCase(command, "stats")) {
                displayStats();
                continue;  // 返回命令输入循环 // Retourner à la boucle des commandes
            }
            else if (Utils::equalIgnoreCase(command, "go")) {
                break;  // Quittez la boucle de saisie de commande et continuez le jeu 退出命令输入循环，继续游戏
            }
            else if (!command.empty()) {
                std::cout << "Commande invalide. Veuillez entrer 'save', 'stats' ou 'go'\n"; // 无效的命令。请输入 'save'、'stats' 或 'go'
                continue;  // Retour à la boucle d'entrée de commande 返回命令输入循环
            }
            // Si l'entrée est vide, continuez la boucle 如果输入为空，继续循环
        }
    }
    
    // Sauvegarder automatiquement à la fin du jeu 游戏结束时自动保存
    std::time_t now = std::time(nullptr);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y%m%d_%H%M%S", localtime(&now));
    std::string autoSaveFilename = "dominion_autosave_" + std::string(timeStr) + ".txt";
    
    std::cout << "\nGame over, création de la sauvegarde automatique...\n"; // 游戏结束，正在创建自动存档...
    saveGame(autoSaveFilename);
    
    // afficher le résultat final 显示最终结果
    auto scores = calculateFinalScores();
    displayResults(scores);
}

void Game::playTurn() {
    Player& currentPlayerRef = players[currentPlayer];
    
    std::cout << "\n========== Tour " << turnCount << " - " 
              << currentPlayerRef.getName() << " ==========\n";
    
    // Afficher l'état du deck 显示牌堆状态
    currentPlayerRef.showDeckStatus();
    
    // Afficher la main actuelle 显示当前手牌
    std::cout << "[" << currentPlayerRef.getName() << "] ";
    currentPlayerRef.showHand();
    
    // étape d'action 行动阶段
    playActionPhase();
    
    // étape d'achat 购买阶段
    playBuyPhase();
    
    // phase de nettoyage 清理阶段
    playCleanupPhase();
    
    // Passer au joueur suivant 切换到下一个玩家
    currentPlayer = (currentPlayer + 1) % players.size();
    
    // Si vous revenez au premier joueur, le numéro du tour est augmenté de 1 如果回到第一个玩家，回合数加1
    if (currentPlayer == 0) {
        turnCount++;
    }
}

void Game::playActionPhase() {
    Player& currentPlayerRef = players[currentPlayer];
    
    std::cout << "\n[Tour " << turnCount << " - Phase d'Action]\n";
    
    while (currentPlayerRef.getActions() > 0) {
        std::cout << "[Tour " << turnCount << "] " 
                 << currentPlayerRef.getName() 
                 << " a: " << currentPlayerRef.getActions() << " points d'action\n\n";
        
        // Afficher la main actuelle 显示当前手牌
        currentPlayerRef.showHand();
        std::cout << "\n";
        
        if (!currentPlayerRef.hasActionCard()) {
            std::cout << "Il n'y a pas de cartes d'action disponibles\n";
            break;
        }
        
        std::cout << "[Tour " << turnCount << "] " 
                 << currentPlayerRef.getName() 
                 << " choisissez une carte d'action à utiliser ('show' pour voir les cartes, 'info [nom de carte]' pour voir la description, 'skip' pour passer): ";
        
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
               continue;        }
        
        // Essayez d'utiliser des cartes mobiles 尝试使用行动卡
        try {
            if (currentPlayerRef.playAction(input, *this)) {
                currentPlayerRef.useAction();
                logger.recordCardPlayed(input);
            }
        } catch (const std::exception& e) {
            std::cout << "Échec de l'utilisation de la carte mobile: " << e.what() << "\n"; // 使用行动卡失败:
            continue;
        }
    }
}

void Game::playBuyPhase() {
    Player& currentPlayerRef = players[currentPlayer];
    
    std::cout << "\n=== Phase d'Achat de " << currentPlayerRef.getName() << "  ===\n";
    currentPlayerRef.playAllTreasures();
    
    while (currentPlayerRef.getBuys() > 0) {
        int availableCoins = currentPlayerRef.getCoins();
        std::cout << "\n[" << currentPlayerRef.getName() << "] Nombre d'achats restants: " 
                 << currentPlayerRef.getBuys() << "\n";
        
        // Afficher les cartes disponibles à l'achat 显示可购买的卡片
        displaySupplyPiles();
        
        // Afficher les pièces disponibles 显示可用金币
        std::cout << "\n[" << currentPlayerRef.getName() << "] Pièces disponibles: " 
                 << availableCoins << "\n";
        
        std::cout << "[" << currentPlayerRef.getName() 
                 << "] Entrez le nom de la carte à acheter, ou 'done' pour terminer la phase d'achat: ";
        
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
                // acheter une carte 购买卡片
                std::shared_ptr<Card> boughtCard = it->second.back();
                it->second.pop_back();
                currentPlayerRef.addToDiscard(boughtCard);
                currentPlayerRef.spendCoins(cardCost);
                currentPlayerRef.useBuy();
                
                logger.recordCardBought(it->first);
                logger.logAction(currentPlayerRef.getName(), 
                    "a acheté " + it->first + " (coût: " + std::to_string(cardCost) + " pièces)");
                
                std::cout << "Achat de " << it->first << " réussi!\n";
                
                if (currentPlayerRef.getBuys() == 0) {
                    std::cout << "Plus de tentatives d'achat disponibles\n";
                    break;
                }
                if (currentPlayerRef.getCoins() == 0) {
                    std::cout << "Plus de pièces disponibles\n";
                    break;
                }
            } else {
                std::cout << "Pas assez de pièces! Il vous faut " << cardCost << " pièces\n";
            }
        } else {
            if (it == supply.end()) {
                std::cout << "Carte introuvable\n";
            } else {
                std::cout << "Carte épuisée\n";
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
    // 1. Vérifiez si le tas Province est vide 检查Province堆是否为空
    if (isProvinceEmpty()) {
        std::cout << "\n*** Fin du jeu: La pile de Province est épuisée! ***\n";
        return true;
    }
    
    // 2. Vérifiez si trois piles de cartes sont vides (y compris tous les types de cartes) 检查是否有任意三堆卡牌堆空（括所有类型的卡）
    int emptyPiles = getEmptyPiles();
    if (emptyPiles >= 3) {
        std::cout << "\n*** Fin du jeu: " << emptyPiles << " piles de cartes sont épuisées! ***\n";
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
    
    // Vérifiez toutes les piles de fournitures, y compris les cartes de base et les cartes de royaume. 检查所有供应堆，包括基础卡和王国卡
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
        
        // Enregistrer les informations de base du jeu 保存基本游戏信息
        saveData["turn_count"] = turnCount;
        saveData["current_player"] = currentPlayer;
        
        // Enregistrer les informations du joueur 保存玩家信息
        json playersJson = json::array();
        for (const auto& player : players) {
            playersJson.push_back(player.toJson());
        }
        saveData["players"] = playersJson;
        
        // Enregistrer l'état du tas d'approvisionnement 保存供应堆状态
        json supplyJson;
        for (const auto& [cardName, pile] : supply) {
            supplyJson[cardName] = pile.size();
        }
        saveData["supply"] = supplyJson;
        
        // Enregistrer le journal du jeu 保存游戏日志
        saveData["game_log"] = logger.toJson();
        
        // écrire un fichier 写入文件
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("Impossible de créer le fichier de sauvegarde: " + filename);
        }
        out << std::setw(4) << saveData << std::endl;
        
        std::cout << "e jeu a été sauvegardé avec succès dans " << filename << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "Erreur lors de la sauvegarde du jeu: " << e.what() << "\n";
        throw;
    }
}

void Game::loadGame(const std::string& filename) {
    std::cout << "\nChargement du jeu...\n";
    
    try {
        // lire le fichier 读取文件
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("Impossible d'ouvrir le fichier de sauvegarde: " + filename);
        }
        
        // Analyser JSON 解析JSON
        json saveData;
        try {
            in >> saveData;
        } catch (const json::exception& e) {
            throw std::runtime_error("Échec de l'analyse du fichier d'archive: " + std::string(e.what())); // 解析存档文件失败:
        }
        
        // Charger les informations de base 加载基本信息
        turnCount = saveData["turn_count"].get<int>();
        currentPlayer = saveData["current_player"].get<int>();
        
        // Charger les informations du joueur 加载玩家信息
        players.clear();
        for (const auto& playerJson : saveData["players"]) {
            try {
                std::string playerName = playerJson["name"].get<std::string>();
                Player player(playerName);
                
                // Définir le statut du joueur 设置玩家状态
                player.setActions(playerJson["actions"].get<int>());
                player.setBuys(playerJson["buys"].get<int>());
                player.setCoins(playerJson["coins"].get<int>());
                
                // Pont de chargement 加载牌组
                for (const auto& cardName : playerJson["deck"]) {
                    try {
                        player.addToDeck(Card::createCard(cardName.get<std::string>()));
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Échec du chargement du deck: " + std::string(e.what())); // 加载牌组失败
                    }
                }
                
                // Charger la main 加载手牌
                for (const auto& cardName : playerJson["hand"]) {
                    try {
                        player.addToHand(Card::createCard(cardName.get<std::string>()));
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Échec du chargement des cartes de main: " + std::string(e.what())); // 加载手牌失败
                    }
                }
                
                // Charger la pile de défausse 加载弃牌堆
                for (const auto& cardName : playerJson["discard"]) {
                    try {
                        player.addToDiscard(Card::createCard(cardName.get<std::string>()));
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Échec du chargement de la pile de défausse: " + std::string(e.what()));  // 加载弃牌堆失败
                    }
                }
                
                players.push_back(std::move(player));
            } catch (const std::exception& e) {
                throw std::runtime_error("Échec du chargement des données du joueur: " + std::string(e.what()));  // 加载玩家数据失败
            }
        }
        
        // Charger le tas d'approvisionnement 加载供应堆
        supply.clear();
        for (const auto& [cardName, count] : saveData["supply"].items()) {
            try {
                supply[cardName] = std::vector<std::shared_ptr<Card>>();
                for (int i = 0; i < count.get<int>(); i++) {
                    supply[cardName].push_back(Card::createCard(cardName));
                }
            } catch (const std::exception& e) {
                throw std::runtime_error("Échec du chargement du tas de réserve: " + cardName + " - " + e.what()); // 加载供应堆失败
            }
        }
        
        // Charger le journal de jeu 加载游戏日志
        if (saveData.contains("game_log")) {
            logger.fromJson(saveData["game_log"]);
        }
        
        std::cout << "Le jeu: " << filename << " est chargé\n";
        std::cout << "Tour actuel: " << turnCount << "\n";
        std::cout << "Player actuel: " << players[currentPlayer].getName() << "\n";
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Échec du chargement du jeu: " + std::string(e.what())); // 加载游戏失败
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
    std::cout << "\n=== jeu terminé ===\n\n";
    std::cout << "score final:\n";
    
    int highestScore = scores[0].second;
    for (const auto& [name, score] : scores) {
        std::cout << name << ": " << score << " point";
        if (score == highestScore) {
            std::cout << " 🏆";
        }
        std::cout << "\n";
    }
}

void Game::displayFinalState() const {
    std::cout << "\n=== Statut de fin de partie ===\n";
    std::cout << "Nombre total de tours: " << turnCount << "\n";
    std::cout << "tas de provisions vide:\n";
    
    for (const auto& [cardName, pile] : supply) {
        if (pile.empty()) {
            std::cout << "- " << cardName << "\n";
        }
    }
    std::cout << "==================\n\n";
}

void Game::displayStats() {
    std::cout << "\n=== statistiques du jeu ===\n";
    
    // Afficher l'état de la réserve 显示供应堆状态
    std::cout << "état du tas de réserve:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte | " 
              << std::setw(10) << "Quantité restante" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    // Affichage regroupé par type 按类型分组显示
    // Montrez d'abord la carte au trésor de base 首先示基础财宝卡
    for (const auto& [cardName, pile] : supply) {
        if (cardName == "Copper" || cardName == "Silver" || cardName == "Gold") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    
    // Affichez ensuite la carte de points de victoire et la carte de malédiction 然后显示胜利点数卡和诅咒卡
    for (const auto& [cardName, pile] : supply) {
        if (cardName == "Estate" || cardName == "Duchy" || 
            cardName == "Province" || cardName == "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    
    // Carte du Royaume montrée en dernier 最后显示王国卡
    for (const auto& [cardName, pile] : supply) {
        if (cardName != "Copper" && cardName != "Silver" && cardName != "Gold" &&
            cardName != "Estate" && cardName != "Duchy" && 
            cardName != "Province" && cardName != "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    
    std::cout << std::string(40, '-') << "\n";
    
    // Affiche l'état du deck de chaque joueur   显示每个玩家的牌堆状态
    std::cout << "\nStatut du deck joueur:\n";
    for (const auto& player : players) {
        std::cout << player.getName() << ": ";
        player.showDeckStatus();
    }
}

void Game::otherPlayersDiscardToThree(const Player& currentPlayer) {
    for (auto& player : players) {
        if (&player != &currentPlayer) {
            // Vérifiez s'il y a une défense contre les douves   检查是否有护城河防御
            if (!player.hasMoat()) {
                int handSize = player.getHandSize();
                if (handSize > 3) {
                    std::cout << "\n" << player.getName() << " Doit être jeté " 
                             << (handSize - 3) << " cartes\n";
                    player.discardDownTo(3);
                }
            } else {
                std::cout << player.getName() << " Douves utilisées pour se défendre contre les attaques\n";
                logger.logAction(player.getName(), "Douves utilisées pour se défendre contre les attaques");
            }
        }
    }
}

void Game::otherPlayersDraw(const Player& currentPlayer, int count) {
    for (auto& player : players) {
        if (&player != &currentPlayer) {
            player.drawCard(count);
            logger.logAction(player.getName(), 
                "Pioché " + std::to_string(count) + " cartes");
        }
    }
}

void Game::gainCardUpToCost(Player& player, int maxCost) {
    std::cout << "\n=== obtenir une carte ===\n";
    std::cout << "Cartes disponibles (dépense maximale: " << maxCost << "):\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte | " 
             << std::setw(10) << "dépenser | " 
             << std::setw(10) << "Quantité restante" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    // Afficher les cartes disponibles   显示可获得的卡片
    for (const auto& [cardName, pile] : supply) {
        if (!pile.empty() && pile[0]->getCost() <= maxCost) {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << pile[0]->getCost()
                     << std::setw(10) << pile.size() << "\n";
        }
    }
    std::cout << std::string(40, '-') << "\n";
    
    std::cout << "\nVeuillez entrer le nom de la carte que vous souhaitez obtenir: ";
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
        logger.logAction(player.getName(), "obtenir " + it->first);
        std::cout << "obtenu avec succès " << it->first << "!\n";
    } else {
        std::cout << "Impossible d'obtenir la carte\n";
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
        std::cout << "Cette carte est introuvable dans la réserve.\n";
    }
}

void Game::displaySupplyPiles() const {
    std::cout << "\n=== état du tas de réserve ===\n";
    
    // Afficher la carte de pièce d'or   显示金币卡
    std::cout << "\nCartes Trésor:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte | " 
              << std::setw(10) << "dépenser | " 
              << std::setw(10) << "Quantité restante" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    const std::vector<std::string> treasureCards = {"Copper", "Silver", "Gold"};
    for (const auto& cardName : treasureCards) {
        if (supply.count(cardName) && !supply.at(cardName).empty()) {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << supply.at(cardName)[0]->getCost()
                     << std::setw(10) << supply.at(cardName).size() << "\n";
        }
    }
    
    // Montrer la carte des points de victoire   显示胜利点数卡
    std::cout << "\nCartes Victoire:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte | " 
              << std::setw(10) << "dépenser | " 
              << std::setw(10) << "Quantité restante" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    const std::vector<std::string> victoryCards = {"Estate", "Duchy", "Province"};
    for (const auto& cardName : victoryCards) {
        if (supply.count(cardName) && !supply.at(cardName).empty()) {
            std::cout << std::setw(15) << cardName 
                     << std::setw(10) << supply.at(cardName)[0]->getCost()
                     << std::setw(10) << supply.at(cardName).size() << "\n";
        }
    }
    
    // Montrer la carte malédiction   显示诅咒卡
    std::cout << "\ncarte de malédiction:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte | " 
              << std::setw(10) << "dépenser | " 
              << std::setw(10) << "Quantité restante" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    if (supply.count("Curse") && !supply.at("Curse").empty()) {
        std::cout << std::setw(15) << "Curse"
                 << std::setw(10) << supply.at("Curse")[0]->getCost()
                 << std::setw(10) << supply.at("Curse").size() << "\n";
    }
    
    // montrer la carte d'action   显示行动卡
    std::cout << "\nCartes Action:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Nom de la carte | " 
              << std::setw(10) << "dépenser | " 
              << std::setw(10) << "Quantité restante" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    for (const auto& [cardName, pile] : supply) {
        // Si ce n'est pas une carte de base ou une carte malédiction, c'est une carte action   如果不是基础卡或诅咒卡，就是行动卡
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
            // Vérifiez s'il y a une défense contre les douves   检查是否有护城河防御
            if (!player.hasMoat()) {
                // Vérifiez s'il y a encore des cartes dans le paquet de malédiction   检查诅咒牌堆是否还有牌
                if (!supply["Curse"].empty()) {
                    player.addToDiscard(supply["Curse"].back());
                    supply["Curse"].pop_back();
                    logger.logAction(player.getName(), "J'ai une carte de malédiction");
                }
            } else {
                logger.logAction(player.getName(), "Douves utilisées pour se défendre contre l'effet de la sorcière");
            }
        }
    }
}

void Game::resolveThiefEffect(Player& currentPlayer) {
    for (auto& player : players) {
        if (&player != &currentPlayer) {
            if (!player.hasMoat()) {
                // Regardez les deux premières cartes du jeu   查看牌库顶的两张牌
                std::vector<std::shared_ptr<Card>> revealedCards;
                auto& playerDeck = player.getDeck();
                
                for (int i = 0; i < 2 && !playerDeck.empty(); i++) {
                    revealedCards.push_back(playerDeck.back());
                    playerDeck.pop_back();
                }
                
                // Afficher les cartes révélées   显示被揭示的牌
                std::cout << player.getName() << " carte révélée: ";
                for (const auto& card : revealedCards) {
                    std::cout << card->getName() << " ";
                }
                std::cout << "\n";
                
                // Trouver la carte au trésor   找出财宝牌
                std::vector<std::shared_ptr<Card>> treasures;
                std::copy_if(revealedCards.begin(), revealedCards.end(),
                    std::back_inserter(treasures),
                    [](const auto& card) { return card->getType() == CardType::TREASURE; });
                
                // S'il y a des cartes trésor, laissez le joueur actuel en choisir une.   如果有财宝牌，让当前玩家选择一张
                if (!treasures.empty()) {
                    std::cout << "Sélectionnez la carte au trésor que vous souhaitez obtenir (entrez le numéro):\n";
                    for (size_t i = 0; i < treasures.size(); i++) {
                        std::cout << i + 1 << ". " << treasures[i]->getName() << "\n";
                    }
                    
                    int choice;
                    std::cin >> choice;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    
                    if (choice > 0 && choice <= treasures.size()) {
                        currentPlayer.addToDiscard(treasures[choice - 1]);
                        logger.logAction(currentPlayer.getName(), 
                             " a obtenu " + treasures[choice - 1]->getName()  + "de " + player.getName());
                    }
                }
                
                // Mettez les cartes restantes dans la pile de défausse   其余的牌放入弃牌堆
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
        std::cout << "Main actuelle:\n";
        const auto& hand = currentPlayer.getHand();
        for (size_t i = 0; i < hand.size(); i++) {
            std::cout << i + 1 << ". " << hand[i]->getName() << "\n";
        }
        
        std::cout << "Sélectionnez la carte à rubuter (entrez 0 pour terminer): ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (choice == 0) break;
        
        if (choice > 0 && choice <= hand.size()) {
            std::string cardName = hand[choice - 1]->getName();
            currentPlayer.trashCardFromHand(choice - 1);
            trashCount++;
            logger.logAction(currentPlayer.getName(), "a rubuté " + cardName);
        }
    }
}

int Game::calculateGardensPoints(const Player& player) const {
    int totalCards = player.getAllCards().size();
    return totalCards / 10;  // Toutes les 10 cartes valent 1 point   每10张牌价值1分
}

void Game::displayKingdomCardInfo(const std::string& cardName) const {
    auto card = Card::createCard(cardName);
    Utils::printDivider();
    std::cout << card->getDescription() << "\n";
    Utils::printDivider();
}

std::vector<std::string> Game::selectKingdomCards() {
    Utils::printHeader("Choisissez la carte du Royaume");
    std::cout << "Veuillez choisir comment initialiser la Kingdom Card:\n";
    std::cout << "1. Sélectionner 10 cartes au hasard\n";
    std::cout << "2. Sélectionner manuellement\n";
    std::cout << "3. Utiliser un ensemble recommandé\n";
    std::cout << "4. Voir les informations détaillées des cartes\n";
    
    std::vector<std::string> selected;
    while (true) {
        std::cout << "\nEntrez votre choix (1-4): ";
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
                std::cout << "Sélection invalide, veuillez réessayer\n";
                continue;
        }
        
        if (selected.size() == 10) {
            break;
        }
    }
    
    // Afficher les résultats de la sélection   显示选择结果
    Utils::printHeader("Carte du Royaume sélectionnée");
    for (const auto& cardName : selected) {
        std::cout << "- " << cardName << "\n";
    }
    
    return selected;
}

std::vector<std::string> Game::manualSelectCards() {
    std::vector<std::string> selected;
    displayAvailableKingdomCards();
    
    std::cout << "\nVeuillez saisir les numéros des 10 cartes dans l'ordre (saisissez -1 pour afficher les détails de la carte, -2 pour resélectionner) :\n";
    while (selected.size() < 10) {
        std::cout << "Il vous reste à choisir  " << (10 - selected.size()) << " cartes : ";
        int index;
        std::cin >> index;
        
        if (index == -1) {
            std::cout << "Entrez le numéro de carte pour afficher les détails : ";
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
            // Vérifier si sélectionné   检查是否已选择
            if (std::find(selected.begin(), selected.end(), 
                ALL_KINGDOM_CARDS[index]) != selected.end()) {
                std::cout << "Cette carte a été sélectionnée, veuillez en sélectionner une autre\n";
                continue;
            }
            selected.push_back(ALL_KINGDOM_CARDS[index]);
        } else {
            std::cout << "Sélection invalide, veuillez réessayer\n";
        }
    }
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return selected;
}

void Game::viewCardDetails() {
    displayAvailableKingdomCards();
    std::cout << "\nEntrez le numéro de la carte pour afficher les détails (entrez -1 pour revenir) : ";
    
    while (true) {
        int index;
        std::cin >> index;
        
        if (index == -1) break;
        
        if (index >= 0 && index < ALL_KINGDOM_CARDS.size()) {
            displayKingdomCardInfo(ALL_KINGDOM_CARDS[index]);
        } else {
            std::cout << "Numéro invalide, veuillez réessayer\n";
        }
        
        std::cout << "\nEntrez le numéro de la carte pour afficher les détails (entrez -1 pour revenir) : ";
    }
}

std::vector<std::string> Game::getRandomKingdomCards(int count) {
    try {
        std::vector<std::string> availableCards;
        // Inclure uniquement les cartes implémentées   只包含已实现的卡片
        for (const auto& cardName : ALL_KINGDOM_CARDS) {
            try {
                // Tester si la carte peut être créée   测试是否可以创建卡片
                Card::createCard(cardName);
                availableCards.push_back(cardName);
            } catch (...) {
                std::cerr << "Attention : ignorer les cartes non implémentées : " << cardName << std::endl;
            }
        }
        
        if (availableCards.size() < count) {
            throw std::runtime_error("Nombre insuffisant de cartes du Royaume disponibles");
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(availableCards.begin(), availableCards.end(), gen);
        availableCards.resize(count);
        return availableCards;
        
    } catch (const std::exception& e) {
        std::cerr << "Échec de la sélection aléatoire des cartes du royaume : " << e.what() << std::endl;
        throw;
    }
}

void Game::displayAvailableKingdomCards() const {
    std::cout << "\nCartes du Royaume disponibles :\n";
    for (size_t i = 0; i < ALL_KINGDOM_CARDS.size(); i++) {
        std::cout << i << ". " << ALL_KINGDOM_CARDS[i] << "\n";
    }
}

void Game::initializeKingdomCards(const std::vector<std::string>& selectedCards) {
    std::cout << "\nCarte du Royaume au choix :\n";
    for (const auto& cardName : selectedCards) {
        supply[cardName] = std::vector<std::shared_ptr<Card>>();
        for (int i = 0; i < 10; i++) {
            supply[cardName].push_back(Card::createCard(cardName));
        }
        std::cout << "- " << cardName << " (10 photos)\n";
    }
}

void Game::displayGameState() const {
    Utils::printHeader("état du jeu");
    std::cout << "Nombre de tours: " << turnCount << "\n";
    std::cout << "joueur actuel: " << players[currentPlayer].getName() << "\n";
    
    // Afficher l'état de la réserve   显示供应堆状态
    displaySupplyStatus();
    
    // Afficher le statut de tous les joueurs   显示所有玩家状态
    Utils::printHeader("statut du joueur");
    for (const auto& player : players) {
        displayPlayerStatus(player);
    }
}

void Game::displaySupplyStatus() const {
    Utils::printHeader("état du tas de réserve");
    
    // Afficher la carte de base   显示基础卡
    std::cout << "Carte de base:\n";
    Utils::printDivider('-', 30);
    for (const auto& [cardName, pile] : supply) {
        if (cardName == "Copper" || cardName == "Silver" || cardName == "Gold" ||
            cardName == "Estate" || cardName == "Duchy" || cardName == "Province" ||
            cardName == "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(5) << pile.size() << " ouvrir\n";
        }
    }
    
    // Afficher la carte du royaume   显示王国卡
    std::cout << "\nCarte nRoyaume:\n";
    Utils::printDivider('-', 30);
    for (const auto& [cardName, pile] : supply) {
        if (cardName != "Copper" && cardName != "Silver" && cardName != "Gold" &&
            cardName != "Estate" && cardName != "Duchy" && cardName != "Province" &&
            cardName != "Curse") {
            std::cout << std::setw(15) << cardName 
                     << std::setw(5) << pile.size() << " ouvrir\n";
        }
    }
}

void Game::displayPlayerStatus(const Player& player) const {
    std::cout << "\njoueur: " << player.getName() << "\n";
    Utils::printDivider('-', 30);
    std::cout << "Nombre de tuiles en main: " << player.getHandSize() << "\n";
    std::cout << "Nombre de piles de cartes: " << player.getDeck().size() << "\n";
    std::cout << "Défausse: " << player.getDiscard().size() << "\n";
    
    if (&player == &players[currentPlayer]) {
        std::cout << "point d'action: " << player.getActions() << "\n";
        std::cout << "Nombre d'achats: " << player.getBuys() << "\n";
        std::cout << "or: " << player.getCoins() << "\n";
        
        std::cout << "\nMain actuelle:\n";
        player.showHand();
    }
}

void Game::displayCurrentPlayerStatus() const {
    const Player& currentPlayerRef = players[currentPlayer];
    Utils::printHeader(currentPlayerRef.getName() + " statut");
    displayPlayerStatus(currentPlayerRef);
}

std::string Game::getKingdomCardSetName() const {
    // Déterminez le nom de l'ensemble en fonction de la carte Royaume actuellement utilisée.   根据当前使用的王国卡判断套装名称
    std::vector<std::string> currentKingdomCards;
    for (const auto& [cardName, pile] : supply) {
        if (cardName != "Copper" && cardName != "Silver" && cardName != "Gold" &&
            cardName != "Estate" && cardName != "Duchy" && cardName != "Province" &&
            cardName != "Curse") {
            currentKingdomCards.push_back(cardName);
        }
    }
    
    // Vérifiez s'il correspond à l'ensemble par défaut   检查是否匹配预设套装
    if (currentKingdomCards == std::vector<std::string>{
        "Village", "Cellar", "Moat", "Woodcutter", "Workshop",
        "Militia", "Market", "Smithy", "CouncilRoom", "Laboratory"}) {
        return "Pack débutant";
    }
    
    if (currentKingdomCards == std::vector<std::string>{
        "Village", "Chapel", "Witch", "Gardens", "Laboratory",
        "Market", "Smithy", "Militia", "Moneylender", "Thief"}) {
        return "Pack avancé";
    }
    
    return "Pack personnalisé";
} 