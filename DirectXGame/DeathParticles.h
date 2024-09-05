#pragma once
#include "Model.h"
#include "assert.h"
#include <array>
#include "WorldTransform.h"
#include <numbers>
#include "MyMath.h"



class DeathParticles {

public:
	void Initialize(Vector3 position, Model* model, ViewProjection* viewProjection );

	void Update();

	void Draw();

	bool GetIsFinished() const { return isFinished_; }

private:

	Model* model_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;

	static inline const uint32_t kNumParticles = 8;
	std::array<WorldTransform, kNumParticles> worldTransforms_;

	// 存在時間
	static inline const float kDuration = 2.0f;
	// 移動の速さ
	static inline const float kSpeed = 0.05f;
	// 分割した１個分の角度
	static inline const float kAngleUnit = 2 * std::numbers::pi_v<float> / kNumParticles;
	//終了フラグ
	bool isFinished_ = false;
	//経過時間カウント
	float counter_ = 0.0f;
	ObjectColor objectColor_;
	Vector4 color_;
};
