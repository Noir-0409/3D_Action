#include "MapChipField.h"
#include <map>
#include <string>
#include <cassert>

namespace {

// CSVの文字列 → マップチップ種別 変換テーブル
std::map<std::string, MapChipType> mapChipTable = {

    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
    {"2", MapChipType::kDamage},
    {"3", MapChipType::kGoal},
};

}

void MapChipField::Initialize() {}

void MapChipField::Update() {}

void MapChipField::Draw() {}

void MapChipField::ResetMapChipData() {

    // データを一度クリア
    mapChipData_.data.clear();

    // 縦方向の行数を確保
	mapChipData_.data.resize(kNumBlockVirtical);

    // 各行に横方向のブロック数を確保
    for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
	
        mapChipDataLine.resize(kNumBlockHorizontal);
    
    }

}

void MapChipField::LoadMapChipCSV(const std::string& filePath) {

    ResetMapChipData();

    // ファイルを開く
    std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

    // CSV全体を文字列として読み込む
    std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();

    file.close();

    // CSVを1行ずつ読み込みマップ配列に反映
    for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
	
    std::string line;
	getline(mapChipCsv, line);

    std::istringstream line_stream(line);

    for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
	
        std::string word;
		getline(line_stream, word, ',');

        // テーブルに存在する値なら変換
        if (mapChipTable.contains(word)) {
		
            mapChipData_.data[i][j] = mapChipTable[word];
        
        }
    
    }
    
    }

}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) { 
    
    // y方向は上が0番目になるように反転
    return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);

}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) { 

    // 横方向の範囲外チェック
    if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
	
    return MapChipType::kBlank;
    
    }

    // 縦方向の範囲外チェック
    if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
	
    return MapChipType::kBlank;
    
    }

    return mapChipData_.data[yIndex][xIndex];

}

uint32_t MapChipField::GetNumBlockVirtical() { return kNumBlockVirtical; }

uint32_t MapChipField::GetNumBlockHorizontal() { return kNumBlockHorizontal; }

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {

	IndexSet indexSet = {};

    // X方向のインデックス計算
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2.0f) / kBlockWidth);

    // Y方向は上下反転してインデックスに変換
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>(position.y + kBlockHeight / 2.0f / kBlockHeight);

	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) { 

    // チップ中心座標を取得
    Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

    // 矩形範囲を設定
    Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;

	return rect;

}
