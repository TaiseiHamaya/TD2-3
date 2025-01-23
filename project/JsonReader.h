#pragma once
#ifndef JSON_READER_H
#define JSON_READER_H

#include <vector>
#include <string>
#include <json.hpp> // JSONライブラリのヘッダーをインクルード


class JsonReader{

	// コンストラクタ
	JsonReader(const std::string& filename);

	// データ取得関数
	const std::vector<std::vector<int>>& getMapTiles() const;
	const std::vector<int>& getPlayerPos() const;
	const std::vector<int>& getChildPos() const;

	// データを読み込む関数
	bool load();
private:
	std::string filename; // JSONファイル名
	std::vector<std::vector<int>> mapTiles; // マップタイル
	std::vector<int> playerPos; // プレイヤーの位置
	std::vector<int> childPos; // 子供の位置
};

#endif // JSON_READER_H


