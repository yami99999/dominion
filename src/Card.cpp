#include "Card.h"
#include "AllCards.h"
#include "BasicCards.h"
#include "ActionCards.h"
#include <stdexcept>
#include <algorithm>

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

std::shared_ptr<Card> Card::createCard(const std::string& cardName) {
    std::string lowerName = cardName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    try {
        if (lowerName == "copper") return std::make_shared<Copper>();
        if (lowerName == "silver") return std::make_shared<Silver>();
        if (lowerName == "gold") return std::make_shared<Gold>();
        if (lowerName == "estate") return std::make_shared<Estate>();
        if (lowerName == "duchy") return std::make_shared<Duchy>();
        if (lowerName == "province") return std::make_shared<Province>();
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
    } catch (const std::exception& e) {
        throw std::runtime_error("创建卡片失败: " + cardName + " - " + e.what());
    }
    
    throw std::runtime_error("未知的卡片: " + cardName);
} 