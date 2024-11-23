#include "Card.h"
#include "AllCards.h" //  引入所有具体卡牌类的定义，如 Copper、Silver、Gold 等。
#include "BasicCards.h"
#include "ActionCards.h"
#include <stdexcept>
#include <algorithm> // 引入算法库，提供 std::transform 等函数。

std::shared_ptr<Card> Card::fromJson(const json& j) {
    try {
        std::string cardName = j["name"].get<std::string>();
        return createCard(cardName);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create card from JSON: " + std::string(e.what()));
    }
}

void Card::serialize(std::ofstream& out) const {
    size_t nameLength = name.length();
    out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    out.write(name.c_str(), nameLength);
    out.write(reinterpret_cast<const char*>(&cost), sizeof(cost));
    out.write(reinterpret_cast<const char*>(&type), sizeof(type));
}

void Card::deserialize(std::ifstream& in) {
    size_t nameLength;
    in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    name.resize(nameLength);
    in.read(&name[0], nameLength);
    in.read(reinterpret_cast<char*>(&cost), sizeof(cost));
    in.read(reinterpret_cast<char*>(&type), sizeof(type));
}




// 静态工厂方法: 根据卡牌名称创建相应的卡牌对象。
std::shared_ptr<Card> Card::createCard(const std::string& cardName) {
    // 转换卡牌名称为小写
    std::string lowerName = cardName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    

    // 比较卡牌名称并创建相应对象
    try {
        // 基础卡
        if (lowerName == "copper") return std::make_shared<Copper>(); // 如果卡牌名称是 "copper"，则创建一个 Copper 对象。
        if (lowerName == "silver") return std::make_shared<Silver>();
        if (lowerName == "gold") return std::make_shared<Gold>();
        
        // 胜利点数卡
        if (lowerName == "estate") return std::make_shared<Estate>();
        if (lowerName == "duchy") return std::make_shared<Duchy>();
        if (lowerName == "province") return std::make_shared<Province>();
        
        // 行动卡
        if (lowerName == "village") return std::make_shared<Village>();
        if (lowerName == "market") return std::make_shared<Market>();
        if (lowerName == "smithy") return std::make_shared<Smithy>();
        if (lowerName == "woodcutter") return std::make_shared<Woodcutter>();
        if (lowerName == "militia") return std::make_shared<Militia>();
        if (lowerName == "councilroom") return std::make_shared<CouncilRoom>();
        if (lowerName == "moneylender") return std::make_shared<Moneylender>();
        if (lowerName == "moat") return std::make_shared<Moat>();
        if (lowerName == "workshop") return std::make_shared<Workshop>();
        if (lowerName == "cellar") return std::make_shared<Cellar>();
        if (lowerName == "curse") return std::make_shared<Curse>();
    } 
    
    catch (const std::exception& e) {
        throw std::runtime_error("创建卡片失败: " + cardName + " - " + e.what());
    }
    
    // 如果卡牌名称不在已知列表中，抛出异常，提示未知的卡牌名称。
    throw std::runtime_error("未知的卡片: " + cardName);
} 