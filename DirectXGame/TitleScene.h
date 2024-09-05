#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
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
	// モデル
	Model* model_ = nullptr;

	bool finished_ = false;
};