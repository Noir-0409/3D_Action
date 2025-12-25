#pragma once
#include <KamataEngine.h>
#include <cstdint>
#include <fstream>
#include <math/Vector3.h>
#include <sstream>
#include <vector>

using namespace KamataEngine;

enum class MapChipType {

	kBlank,
	kBlock,
	kDamage,
	kGoal,

};

/// <summary>
/// マップチップを管理するクラス
/// </summary>
class MapChipField {

public:
	struct MapChipData {

		std::vector<std::vector<MapChipType>> data;
	};

	struct IndexSet {

		uint32_t xIndex;
		uint32_t yIndex;
	};

	struct Rect {

		float left;
		float right;
		float bottom;
		float top;
	};


	void Initialize();

	void Update();

	void Draw();

	//データをリセット
	void ResetMapChipData();

	//CSVを読み取って配置
	void LoadMapChipCSV(const std::string& filePath);

	//頂点の座標
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	
	//ブロックの種類判別
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	//縦の数
	uint32_t GetNumBlockVirtical();

	//横の数
	uint32_t GetNumBlockHorizontal();

	static inline const uint32_t kNumBlockVirtical = 30;
	static inline const uint32_t kNumBlockHorizontal = 200;

	//マップチップの座標
	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	//4辺の座標
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	MapChipData mapChipData_;
};