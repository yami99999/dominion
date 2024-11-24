#pragma once
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include "Card.h"
#include "BasicCards.h"
#include "ActionCards.h"
#include "Player.h"
#include "Game.h"
#include "Utils.h"
#include "GameLogger.h"

// Assurez-vous que tous les types de cartes sont correctement inclus  确保所有卡片类型都被正确包含
using CardPtr = std::shared_ptr<Card>;
