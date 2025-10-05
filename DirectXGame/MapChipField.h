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

};

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

	void ResetMapChipData();

	void LoadMapChipCSV(const std::string& filePath);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVirtical();

	uint32_t GetNumBlockHorizontal();

	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	MapChipData mapChipData_;
};