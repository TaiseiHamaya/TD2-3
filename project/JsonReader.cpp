#include "JsonReader.h"
#include <iostream>
#include <fstream>
#include <json.hpp>

using json = nlohmann::json;

// コンストラクタ
JsonReader::JsonReader(const std::string& filename) : filename(filename){}

// JSONデータを読み込む
bool JsonReader::load(){
    std::ifstream file(filename);
    if(!file.is_open()) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return false;
    }

    json jsonData;
    file >> jsonData;
    file.close();

    // mapTilesのデータを読み込む
    for(const auto& row : jsonData["map"]["tiles"]) {
        mapTiles.push_back(row.get<std::vector<int>>());
    }

    // playerPosを読み込む
    playerPos = {
        jsonData["player"]["column"],
        jsonData["player"]["row"]
    };

    // childPosを読み込む
    childPos = {
        jsonData["child"]["column"],
        jsonData["child"]["row"]
    };

    return true;
}

// mapTilesを取得
const std::vector<std::vector<int>>& JsonReader::getMapTiles() const{
    return mapTiles;
}

// playerPosを取得
const std::vector<int>& JsonReader::getPlayerPos() const{
    return playerPos;
}

// childPosを取得
const std::vector<int>& JsonReader::getChildPos() const{
    return childPos;
}
