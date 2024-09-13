#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "imgui.h"
#include "Skydome.h"
#include "Sprite.h"
#include <Model.h>
#include "WorldTransform.h"
#include <assert.h>
#include "ViewProjection.h"

class Door {
public:
	Door();

	~Door();

	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();


private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	WorldTransform worldTransform_;            // ワールド変換データ
	Model* model_ = nullptr;                   // モデル
	ViewProjection* viewProjection_ = nullptr; // ViewProjection

	Vector3 position_; // プレイヤーの現在位置
};