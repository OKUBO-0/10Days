#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Skydome.h"
#include "Sprite.h"
#include <Model.h>
#include "WorldTransform.h"
#include <assert.h>
#include "ViewProjection.h"

class TitleScene {
public:

	TitleScene();
	~TitleScene();

	void Initialize();

	void Update();

	void Draw();

	bool GetISFinished() const { return finished_; }


private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;


	// ワールド変換データ
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// モデル
	Model* model_ = nullptr;
	Model* stage1model_ = nullptr;
	Model* stage2model_ = nullptr;
	Model* stage3model_ = nullptr;

	bool finished_ = false;

	//最初の角度[度]
	static inline const float kWalkMotionAngleStart = 5.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = -5.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalklMotionTime = 1.0f;
	//タイマー
	float Timer_ = 0.0f;

	// SkyDome
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
};