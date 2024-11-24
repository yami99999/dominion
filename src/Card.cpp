#include "Card.h"
#include "AllCards.h"
#include <stdexcept>

std::shared_ptr<Card> Card::fromJson(const json& j) {
    try {
        std::string cardName = j["name"].get<std::string>();
        return createCard(cardName);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create card from JSON: " + std::string(e.what()));
    }
}

std::shared_ptr<Card> Card::createCard(const std::string& cardName) {
    try {
        if (cardName == "Copper") return std::make_shared<Copper>();
        if (cardName == "Silver") return std::make_shared<Silver>();
        if (cardName == "Gold") return std::make_shared<Gold>();
        
        if (cardName == "Estate") return std::make_shared<Estate>();
        if (cardName == "Duchy") return std::make_shared<Duchy>();
        if (cardName == "Province") return std::make_shared<Province>();
        if (cardName == "Gardens") return std::make_shared<Gardens>();
        if (cardName == "Curse") return std::make_shared<Curse>();
        
        if (cardName == "Village") return std::make_shared<Village>();
        if (cardName == "Market") return std::make_shared<Market>();
        if (cardName == "Smithy") return std::make_shared<Smithy>();
        if (cardName == "Woodcutter") return std::make_shared<Woodcutter>();
        if (cardName == "Militia") return std::make_shared<Militia>();
        if (cardName == "CouncilRoom") return std::make_shared<CouncilRoom>();
        if (cardName == "Moneylender") return std::make_shared<Moneylender>();
        if (cardName == "Moat") return std::make_shared<Moat>();
        if (cardName == "Workshop") return std::make_shared<Workshop>();
        if (cardName == "Cellar") return std::make_shared<Cellar>();
        if (cardName == "Chapel") return std::make_shared<Chapel>();
        if (cardName == "Witch") return std::make_shared<Witch>();
        if (cardName == "Thief") return std::make_shared<Thief>();
        if (cardName == "Feast") return std::make_shared<Feast>();
        if (cardName == "Laboratory") return std::make_shared<Laboratory>();
        
        throw std::runtime_error("Unknown card type: " + cardName);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create card: " + cardName + " - " + e.what());
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