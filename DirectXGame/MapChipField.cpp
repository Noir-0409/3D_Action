#include "MapChipField.h"
#include <cassert>
#include <map>
#include <string>

namespace KamataEngine { // ★ namespace の開始

namespace {

// CSVの文字列 → マップチップ種別 変換テーブル
std::map<std::string, MapChipType> mapChipTable = {

    {"0", MapChipType::kBlank },
    {"1", MapChipType::kBlock },
    {"2", MapChipType::kDamage},
    {"3", MapChipType::kGoal  },
    {"4", MapChipType::kIce   },
    {"5", MapChipType::kRed   },
    {"6", MapChipType::kBlue  },
};

} // namespace

void MapChipField::Initialize() {}

void MapChipField::Update() {

	blinkTimer_ += 1.0f / 60.0f;

	if (blinkTimer_ >= kBlinkInterval) {
		blinkTimer_ = 0.0f;

		if (blinkPhase_ == BlinkPhase::Red) {
			blinkPhase_ = BlinkPhase::Blue;
		} else {
			blinkPhase_ = BlinkPhase::Red;
		}
	}
}

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

	// 範囲外チェック
	if (xIndex >= kNumBlockHorizontal || yIndex >= kNumBlockVirtical) {
		return MapChipType::kBlank;
	}

	MapChipType type = mapChipData_.data[yIndex][xIndex];

	// --- 赤・青 切り替えブロック制御 ---
	if (type == MapChipType::kRed) {
		// 今が赤フェーズじゃなければ存在しない
		if (blinkPhase_ != BlinkPhase::Red) {
			return MapChipType::kBlank;
		}
	}

	if (type == MapChipType::kBlue) {
		// 今が青フェーズじゃなければ存在しない
		if (blinkPhase_ != BlinkPhase::Blue) {
			return MapChipType::kBlank;
		}
	}

	return type;
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

MapChipType MapChipField::GetRawMapChipTypeByIndex(uint32_t x, uint32_t y) {

	if (x >= kNumBlockHorizontal || y >= kNumBlockVirtical) {
		return MapChipType::kBlank;
	}
	return mapChipData_.data[y][x];
}

} // namespace KamataEngine