#pragma once
#include "Vector3.h"
#include <assert.h>
#include <stdint.h>
#include <string>
#include <vector>

enum class MapChipType {

	kBlank,  // 空白
	kBlock,  // ブロック
	kBlock2, // ブロック
	kDoor,   // ドア
};
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

class MapChipField {


public:

	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	Vector3 GetMapChipPostionByIndex(uint32_t xIndex, uint32_t yIndex);
	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }
	IndexSet GetMapChipIndexSetByPosition(const Vector3& posotopn);
	Rect GetRectByIndex(uint32_t xindex, uint32_t yIndex);
	void SetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex, MapChipType type);  // 新しく追加
	void InvertMap();

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロック個数
	static inline const uint32_t kNumBlockVirtical = 40;
	static inline const uint32_t kNumBlockHorizontal = 40;
	MapChipData mapChipData_;

};