#include "Card.h"
#include "AllCards.h"
#include <stdexcept>

// Charger les noms de cartes depuis JSON et créer des cartes
//  从 JSON 加载卡牌名称并创建卡牌
std::shared_ptr<Card> Card::fromJson(const json &j)
{
    try
    {
        // Récupère le nom de la carte
        //  获取卡牌名称
        std::string cardName;
        if (j.is_string())
        {
            cardName = j.get<std::string>();
        }
        else if (j.contains("name"))
        {
            cardName = j["name"].get<std::string>();
        }
        else
        {
            throw std::runtime_error("Invalid card format in JSON");
        }
        // Créer une carte
        //  创建卡牌
        return createCard(cardName);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Failed to create card from JSON: " + std::string(e.what()));
    }
}

// Créer une carte en fonction de son nom
// Utiliser le mode usine pour créer des cartes : créer tous types de cartes de manière centralisée
// Pointeur : pointeur intelligent pointant vers l'objet de classe Card
//  根据名称创建卡牌
//  使用工厂模式创建卡牌: 集中创建所有类型的卡牌
//  指针: 指向Card类对象的智能指针
std::shared_ptr<Card> Card::createCard(const std::string &cardName)
{
    try
    { // Essayez de créer une carte
        // 尝试创建卡牌

        // 1. Cartes au trésor de base : pièces de cuivre, pièces d'argent, pièces d'or
        //  1.基础财宝牌：铜币、银币、金币
        if (cardName == "Copper")
            return std::make_shared<Copper>();
        if (cardName == "Silver")
            return std::make_shared<Silver>();
        if (cardName == "Gold")
            return std::make_shared<Gold>();

        // 2. Cartes de points de victoire de base : manoir, duc, province, jardin, malédiction
        //  2.基础胜利点数牌：庄园、公爵、省份、花园、诅咒
        if (cardName == "Estate")
            return std::make_shared<Estate>();
        if (cardName == "Duchy")
            return std::make_shared<Duchy>();
        if (cardName == "Province")
            return std::make_shared<Province>();
        if (cardName == "Gardens")
            return std::make_shared<Gardens>();
        if (cardName == "Curse")
            return std::make_shared<Curse>();

        // 3. Cartes d'action de base : village, marché, artisan, ferme forestière, milice, maison municipale, changeur d'argent, tranchée, atelier, cave à vin, sorcière, voleur, festin, laboratoire
        //  3.基础行动牌：村庄、市场、工匠、林场、民兵、议事厅、钱币兑换商、壕沟、工坊、酒窖、女巫、窃贼、盛宴、实验室
        if (cardName == "Village")
            return std::make_shared<Village>();
        if (cardName == "Market")
            return std::make_shared<Market>();
        if (cardName == "Smithy")
            return std::make_shared<Smithy>();
        if (cardName == "Woodcutter")
            return std::make_shared<Woodcutter>();
        if (cardName == "Militia")
            return std::make_shared<Militia>();
        if (cardName == "CouncilRoom")
            return std::make_shared<CouncilRoom>();
        if (cardName == "Moneylender")
            return std::make_shared<Moneylender>();
        if (cardName == "Moat")
            return std::make_shared<Moat>();
        if (cardName == "Workshop")
            return std::make_shared<Workshop>();
        if (cardName == "Cellar")
            return std::make_shared<Cellar>();
        if (cardName == "Chapel")
            return std::make_shared<Chapel>();
        if (cardName == "Witch")
            return std::make_shared<Witch>();
        if (cardName == "Thief")
            return std::make_shared<Thief>();
        if (cardName == "Feast")
            return std::make_shared<Feast>();
        if (cardName == "Laboratory")
            return std::make_shared<Laboratory>();

        // 4. Autres cartes
        //  4.其他牌
        throw std::runtime_error("Unknown card type: " + cardName);
    }

    // Échec de la création de la carte, erreur générée
    //  创建卡牌失败, 抛出错误
    catch (const std::exception &e)
    {
        throw std::runtime_error("Failed to create card: " + cardName + " - " + e.what());
    }
}

// inutile
//////////////////////////////////////////////////////////////////////////
// void Card::serialize(std::ofstream& out) const { //Sérialiser la carte
// size_t nameLength = name.length(); //Récupère la longueur du nom de la carte
// out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength)); //Écrit la longueur du nom de la carte
// out.write(name.c_str(), nameLength); //Écrivez le nom de la carte
// out.write(reinterpret_cast<const char*>(&cost), sizeof(cost)); //Coût de la carte d'écriture
// out.write(reinterpret_cast<const char*>(&type), sizeof(type)); //Écrire le type de carte
// }
//  useless
///////////////////////////////////////////////////////////////////////
// void Card::serialize(std::ofstream& out) const { // 序列化卡牌
//     size_t nameLength = name.length(); // 获取卡牌名称长度
//     out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength)); // 写入卡牌名称长度
//     out.write(name.c_str(), nameLength); // 写入卡牌名称
//     out.write(reinterpret_cast<const char*>(&cost), sizeof(cost)); // 写入卡牌费用
//     out.write(reinterpret_cast<const char*>(&type), sizeof(type)); // 写入卡牌类型
// }

////Désérialiser les cartes (lire les données de la carte à partir du fichier)
// void Card::deserialize(std::ifstream& in) {
// size_t nameLength; //Longueur du nom de la carte
// in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength)); //Lit la longueur du nom de la carte
// name.resize(nameLength); //Ajuste la longueur du nom de la carte
// in.read(&name[0], nameLength); //Lit le nom de la carte
// in.read(reinterpret_cast<char*>(&cost), sizeof(cost)); //Lire le coût de la carte
// in.read(reinterpret_cast<char*>(&type), sizeof(type)); //Lire le type de carte
// }
//  // 反序列化卡牌(从文件中读取卡牌数据)
//  void Card::deserialize(std::ifstream& in) {
//      size_t nameLength; // 卡牌名称长度
//      in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength)); // 读取卡牌名称长度
//      name.resize(nameLength); // 调整卡牌名称长度
//      in.read(&name[0], nameLength); // 读取卡牌名称
//      in.read(reinterpret_cast<char*>(&cost), sizeof(cost)); // 读取卡牌费用
//      in.read(reinterpret_cast<char*>(&type), sizeof(type)); // 读取卡牌类型
//  }