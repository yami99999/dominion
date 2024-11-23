#include "Player.h" // 引入本身的头文件，实现类的成员函数。
#include "Game.h" // 用于与游戏逻辑交互。
#include "AllCards.h" // 引入所有卡牌的定义，用于创建和管理卡牌对象。
#include "BasicCards.h" //  引入基础卡牌和行动卡牌的定义。
#include "ActionCards.h" 
#include <algorithm> // 引入算法库，提供函数如 std::find_if、std::any_of、std::sort 等。
#include <random> // 引入随机数生成库，用于洗牌。 
#include <iostream> // 引入标准输入输出库，用于控制台输出。
#include <fstream> // 引入文件输入输出流，用于保存和加载玩家状态。



// 构造函数
Player::Player(const std::string& playerName) : 
    // 初始化玩家的名称为传入的 `playerName`
    // 初始化行动点数为1
    // 初始化购买次数为1
    // 初始化金币数为0
    name(playerName), actions(1), buys(1), coins(0) { // 所有初始化都在初始化列表中完成
}


// 玩家从抽牌堆中抽取指定数量的卡牌，默认抽1张。
void Player::drawCard(int count) {
    // 执行 count 次抽牌操作
    for (int i = 0; i < count; i++) {
        // 检查抽牌堆是否为空
        if (deck.empty()) {
            // 如果抽牌堆为空，检查弃牌堆是否也为空。
            if (discard.empty()) return; // 如果弃牌堆也为空，无法继续抽牌，直接返回。
            shuffleDeck(); //  如果弃牌堆不为空，调用 shuffleDeck 方法，将弃牌堆中的卡牌洗入抽牌堆。
        }



        hand.push_back(deck.back()); // 将抽牌堆的最后一张卡牌（堆顶）添加到手牌中。
        deck.pop_back(); // 从抽牌堆中移除这张卡牌。
    }
}





// 将弃牌堆中的卡牌重新洗入抽牌堆

void Player::shuffleDeck() {
    // 将弃牌堆中的所有卡牌添加到抽牌堆的末尾。
    // 合并牌堆
    deck.insert(deck.end(), discard.begin(), discard.end());
    //  清空弃牌堆
    discard.clear();
    

    // 洗牌操作
    std::random_device rd; // 创建一个随机数生成器。
    std::mt19937 g(rd()); // 使用梅森旋转算法引擎初始化随机数生成器。
    std::shuffle(deck.begin(), deck.end(), g); // 使用生成器 `g` 对抽牌堆进行洗牌
}




// 将玩家的手牌中的所有卡牌弃置到弃牌堆中。
void Player::discardHand() {
    // 将手牌中的所有卡牌添加到弃牌堆的末尾。
    discard.insert(discard.end(), hand.begin(), hand.end());
    //  清空手牌
    hand.clear();
}








// 初始化玩家的起始牌组，包括基础财宝卡和胜利点数卡。
void Player::initializeDeck() {

    // 捕捉在初始化过程中可能发生的异常（如卡牌创建失败）
    try {
        // 初始牌组：7张铜币和3张庄园

        // 创建7张铜币卡牌，并添加到抽牌堆中
        for (int i = 0; i < 7; i++) {
            deck.push_back(Card::createCard("Copper"));
        }

        // 创建3张庄园卡牌，并添加到抽牌堆中。
        for (int i = 0; i < 3; i++) {
            deck.push_back(Card::createCard("Estate"));
        }
        shuffleDeck(); // 洗牌，将抽牌堆中的卡牌打乱。
        drawCard(5); // 玩家抽取5张牌作为起始手牌。
    } 
    
    catch (const std::exception& e) {
        std::cerr << "Échec de l'initialisation du deck.: " << e.what() << std::endl; // 初始化牌组失败
        throw; // 重新抛出异常，以便上层调用者处理
    }
}


// 显示玩家的抽牌堆和弃牌堆的数量 
void Player::showDeckStatus() const {
    // 输出抽牌堆和弃牌堆中卡牌的数量
    std::cout << "[État des piles de cartes] Pioche : " << deck.size() << " cartes, "
              << "Défausse : " << discard.size() << " cartes\n";
}






// 显示玩家当前手牌的内容，按卡牌类型分类显示
void Player::showHand() const {
    // 输出手牌的数量
    std::cout << "Main (" << hand.size() << " cartes): ";
    
    // 用于控制逗号的输出，避免在第一个卡牌前输出逗号。
    bool first = true;
    
    // 按类型分类显示
        // 首先显示财宝卡
        // 遍历手牌，找出所有财宝卡（如铜币、银币、金币）
    for (const auto& card : hand) {
        if (card->getType() == CardType::TREASURE) {
            // 如果不是第一个卡牌，先输出逗号。
            if (!first) std::cout << ", ";
            // 输出卡牌名称。
            std::cout << card->getName();
            // 标记已输出至少一个卡牌。
            first = false;
        }
    }
    
    // 然后显示行动卡
        // 类似财宝卡的处理，输出所有行动卡的名称。
    for (const auto& card : hand) {
        if (card->getType() == CardType::ACTION) {
            if (!first) std::cout << ", ";
            std::cout << card->getName();
            first = false;
        }
    }
    
    // 最后显示胜利点数卡和诅咒卡
        // 输出所有胜利点数卡和诅咒卡的名称
    for (const auto& card : hand) {
        if (card->getType() == CardType::VICTORY) {
            if (!first) std::cout << ", ";
            std::cout << card->getName();
            first = false;
        }
    }
    
    std::cout << "\n";
}






// 检查玩家的手牌中是否有行动卡
bool Player::hasActionCard() const {

    // 使用 std::any_of 遍历手牌，检查是否有任何一张卡牌的类型为 CardType::ACTION
    return std::any_of(hand.begin(), hand.end(),
        [](const auto& card) { return card->getType() == CardType::ACTION; });
}



// 尝试使用一张指定名称的行动卡
bool Player::playAction(const std::string& cardName, Game& game) {
    // 查找指定的行动卡
    // 在手牌中查找名称与 `cardName` 匹配的卡牌
    auto it = std::find_if(hand.begin(), hand.end(),
        [&cardName](const auto& card) { return card->getName() == cardName; });
    
    // 检查卡牌是否存在
    // 如果未找到卡牌，输出提示并返回 `false`
    if (it == hand.end()) {
        std::cout << "Ne trouve pas la carte.!\n";
        return false;
    }
    // 检查卡牌类型
    //  如果找到的卡牌类型不是行动卡，输出提示并返回 `false`。
    if ((*it)->getType() != CardType::ACTION) {
        std::cout << "这不是行动卡!\n";
        return false;
    }
    
    // 使用卡牌
    (*it)->play(*this, game); // 调用卡牌的 `play` 方法，传入当前玩家和游戏实例，执行卡牌的效果。
    discard.push_back(*it); // 将使用过的卡牌添加到弃牌堆
    hand.erase(it); // 从手牌中移除这张卡牌
    return true;
}




// 将一张卡牌添加到弃牌堆中
void Player::addToDiscard(std::shared_ptr<Card> card) {
    discard.push_back(card); // 将卡牌添加到弃牌堆的末尾
}









// 将玩家的当前状态保存到输出文件流中
void Player::saveState(std::ofstream& out) const {
    // 保存玩家名称
    size_t nameLength = name.length(); // 获取玩家名称的长度。
    out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength)); // 将名称长度写入文件
    out.write(name.c_str(), nameLength);// 将名称内容写入文件
    // 保存资源数量
    out.write(reinterpret_cast<const char*>(&actions), sizeof(actions)); // 写入行动点数。
    out.write(reinterpret_cast<const char*>(&buys), sizeof(buys)); // 写入购买次数
    out.write(reinterpret_cast<const char*>(&coins), sizeof(coins)); // 写入金币数
    
    // 保存牌组信息
    // TODO: 实现牌组序列化
}





// 从输入文件流中加载玩家的状态
void Player::loadState(std::ifstream& in) {
    // 读取玩家名称
    size_t nameLength; // 声明变量存储名称长度
    in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength)); // 从文件中读取名称长度。
    name.resize(nameLength); // 调整 `name` 字符串的大小
    in.read(&name[0], nameLength); // 从文件中读取名称内容。
    
    // 读取资源数量
    in.read(reinterpret_cast<char*>(&actions), sizeof(actions)); // 读取行动点数。
    in.read(reinterpret_cast<char*>(&buys), sizeof(buys)); // 读取购买点数
    in.read(reinterpret_cast<char*>(&coins), sizeof(coins)); // 读取金币数
    
    // 读取牌组信息
    // TODO: 实现牌组反序列化
}



// 尝试使用一张指定名称的财宝卡
bool Player::playTreasure(const std::string& cardName) {

    // 查找指定的财宝卡
    // 在手牌中查找名称与 `cardName` 匹配的卡牌
    auto it = std::find_if(hand.begin(), hand.end(),
        [&cardName](const auto& card) { return card->getName() == cardName; });
    
    // 检查卡牌是否存在
    // 如果未找到卡牌，返回 `false`。
    if (it == hand.end()) {
        return false;
    }
    
    // 检查卡牌类型
    // 如果找到的卡牌类型不是财宝卡，返回 `false`。
    if ((*it)->getType() != CardType::TREASURE) {
        return false;
    }
    
    // 创建一个临时Game对象，用于执行财宝卡的 `play` 方法, 因为财富卡不使用游戏状态
    static Game dummyGame(0);  // 使用静态对象避免重复创建
    (*it)->play(*this, dummyGame); // 调用卡牌的 `play` 方法，传入当前玩家和临时游戏实例，执行卡牌的效果（如增加金币）。
    discard.push_back(*it); // 将使用过的卡牌添加到弃牌堆
    hand.erase(it); // 从手牌中移除这张卡牌
    return true;
}




// 获取玩家手牌中的所有财宝卡
std::vector<std::shared_ptr<Card>> Player::getTreasureCards() const {

    // 初始化财宝卡向量(数组)
    std::vector<std::shared_ptr<Card>> treasures;
    // 遍历手牌
    for (const auto& card : hand) {
        // 如果卡牌类型为财宝卡，则将其添加到 `treasures` 向量中
        if (card->getType() == CardType::TREASURE) {
            treasures.push_back(card);
        }
    }
    // 返回结果
    return treasures;
}





// 自动使用玩家手牌中的所有财宝卡（铜币、银币、金币）
void Player::playAllTreasures() {

    // 获取财宝卡
    auto treasures = getTreasureCards(); // 调用 `getTreasureCards` 方法，获取手牌中的所有财宝卡
    // 检查是否有财宝卡
    if (!treasures.empty()) {
        std::cout << "Utilisation automatique de toutes les cartes de trésor:\n"; // 如果有财宝卡，输出提示信息。
    }
    
    // 使用每一张财宝卡
    // 遍历所有财宝卡
    for (const auto& card : treasures) {
        // 从手牌中移除
        // 在手牌中查找当前财宝卡的位置
        auto it = std::find(hand.begin(), hand.end(), card);
        // 如果卡牌存在，执行以下操作
        if (it != hand.end()) {
            // 使用卡片
            static Game dummyGame(0);  // 创建一个静态的临时 `Game` 对象，用于执行财宝卡的 `play` 方法。
            card->play(*this, dummyGame); // 调用卡牌的 `play` 方法，传入当前玩家和临时游戏实例，执行卡牌的效果（如增加金币）。
            
            // 移到弃牌堆
            discard.push_back(*it); // 将使用过的卡牌添加到弃牌堆。
            hand.erase(it); // 从手牌中移除这张卡牌。
            


            // 输出使用信息
            std::cout << "Utilise: " << card->getName() << " (+"; // 输出使用的卡牌名称和效果。
            if (card->getName() == "Copper") std::cout << "1"; // 如果卡牌是铜币，输出增加1金币。
            else if (card->getName() == "Silver") std::cout << "2"; // 如果卡牌是银币，输出增加2金币。
            else if (card->getName() == "Gold") std::cout << "3"; // 如果卡牌是金币，输出增加3金币。
            std::cout << " Pièces)\n";
        }
    }
    
    // 显示当前总金币
    if (!treasures.empty()) { // 如果有财宝卡被使用，输出玩家当前的总金币数
        std::cout << "actuel total : " << coins << "\n"; 
    }
}


// 检查玩家手牌中是否有护城河卡牌（用于防御攻击）
bool Player::hasMoat() const {
    // 遍历手牌
    for (const auto& card : hand) {
        // 如果手牌中存在名称为 "Moat" 的卡牌，返回 `true`
        if (card->getName() == "Moat") {
            return true;
        }
    }
    // 如果遍历结束仍未找到护城河卡牌，返回 `false`
    return false;
}



// 尝试废弃玩家手牌中的一张铜币卡牌
bool Player::trashCopperFromHand() {
    // 查找铜币卡牌
    auto it = std::find_if(hand.begin(), hand.end(),
        [](const auto& card) { return card->getName() == "Copper"; });
    
    // 检查卡牌是否存在
    // 存在则废弃卡牌
    if (it != hand.end()) {
        trash.push_back(*it);  // 将铜币卡牌添加到废弃堆中。
        hand.erase(it); // 从手牌中移除这张铜币卡牌
        return true;
    }
    // 如果未找到铜币卡牌，返回 `false`。
    return false;
}





// 让玩家弃掉手牌中的卡牌，直到手牌数量不超过指定的 `count`。
void Player::discardDownTo(int count) {
    
    
    // 循环弃牌: 当手牌数量超过 `count` 时，持续执行弃牌操作。
    while (hand.size() > count) {
        std::cout << "Main actuelle : "; // 当前手牌
        // 遍历手牌，按编号显示每张卡牌。
        for (size_t i = 0; i < hand.size(); i++) {
            std::cout << i + 1 << "." << hand[i]->getName() << " ";
        }
        


        // 读取玩家选择
        std::cout << "\nVeuillez choisir le numéro des cartes à défausser : "; // 请选择要弃掉的卡片编号
        int choice;
        std::cin >> choice;
        
        // 验证选择并弃牌
        if (choice > 0 && choice <= hand.size()) {// 如果选择的编号有效，将对应的卡牌添加到弃牌堆，并从手牌中移除
            discard.push_back(hand[choice - 1]);
            hand.erase(hand.begin() + choice - 1);
        }
    }
}






// 让玩家弃掉任意数量的卡牌，然后再抽取相同数量的卡牌
int Player::discardAndDraw() {

    // 显示当前手牌
    std::cout << "Main actuelle: ";
    // 遍历手牌，按编号显示每张卡牌
    for (size_t i = 0; i < hand.size(); i++) {
        std::cout << i + 1 << "." << hand[i]->getName() << " ";
    }
    
    // 读取玩家选择
    std::cout << "\nEntrez les numéros des cartes à défausser (séparés par un espace, entrez 0 pour terminer) : ";// 请输入要弃掉的卡片编号（用空格分隔，输入0结束）
    int count = 0;
    std::vector<int> choices;
    int choice;
    
    // 持续读取玩家输入的卡牌编号，直到输入 `0`
    while (std::cin >> choice && choice != 0) {
        // 验证编号有效后，将其添加到 `choices` 向量中，并计数。
        if (choice > 0 && choice <= hand.size()) {
            choices.push_back(choice - 1);
            count++;
        }
    }
    
    // 从大到小排序，以便从后往前删除
    std::sort(choices.rbegin(), choices.rend());
    

    // 遍历排序后的编号，依次将卡牌添加到弃牌堆并从手牌中移除。
    for (int idx : choices) {
        discard.push_back(hand[idx]);
        hand.erase(hand.begin() + idx);
    }
    
    // 抽取与弃牌数量相同的卡牌。
    drawCard(count);


    // 返回弃牌数量
    return count;
}





// 获取玩家所有牌组中的卡牌（抽牌堆、手牌、弃牌堆）
std::vector<std::shared_ptr<Card>> Player::getAllCards() const {
    
    // 初始化所有卡牌向量
    std::vector<std::shared_ptr<Card>> allCards;
    // 合并所有牌组中的卡片
    allCards.insert(allCards.end(), deck.begin(), deck.end()); // 添加抽牌堆中的所有卡牌。
    allCards.insert(allCards.end(), hand.begin(), hand.end()); // 添加手牌中的所有卡牌。
    allCards.insert(allCards.end(), discard.begin(), discard.end()); // 添加弃牌堆中的所有卡牌。
    // 返回合并后的卡牌向量
    return allCards;
}




// 弃掉手牌中指定索引的卡牌
void Player::discardCard(int index) {

    // 验证索引有效性
    if (index >= 0 && index < hand.size()) {
        // 如果索引在有效范围内，执行以下弃牌操作:
        discard.push_back(hand[index]); // 将指定索引的卡牌添加到弃牌堆。
        hand.erase(hand.begin() + index); //  从手牌中移除这张卡牌。
    }
}



// 弃掉玩家手牌中的一张地窖卡牌。
void Player::discardCellar() {
    // 找到并弃置Cellar
    auto it = std::find_if(hand.begin(), hand.end(),
        [](const auto& card) { return card->getName() == "Cellar"; });
    // 检查卡牌是否存在
    if (it != hand.end()) {
        // 如果找到地窖卡牌，执行以下弃牌操作: 
        discard.push_back(*it);
        hand.erase(it);
    }
}



// 显示指定名称的卡牌的描述。
void Player::showCardDescription(const std::string& cardName) const {
    // 遍历手牌 - 先在手牌中查找
    for (const auto& card : hand) {
        // 如果找到名称与 `cardName` 匹配的卡牌
        // 显示卡牌描述
        if (Utils::equalIgnoreCase(card->getName(), cardName)) {
            std::cout << "\n" << card->getDescription() << "\n";
            return;
        }
    }
    // 如果遍历结束仍未找到指定卡牌，输出提示信息。
    std::cout << "Cette carte est introuvable... \n";
}