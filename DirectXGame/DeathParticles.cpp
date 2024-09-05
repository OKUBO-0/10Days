#include "DeathParticles.h"
#include "Player.h"

void DeathParticles::Initialize(Vector3 position, Model* model, ViewProjection* viewProjection) {
	assert(model);
	model_ = model;
	viewProjection_ = viewProjection;

	for (auto& worldTransform : worldTransforms_) {

		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	objectColor_.Initialize();
	color_ = Vector4{1, 1, 1, 1};
}

void DeathParticles::Update() {

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};
		// 回転角を計算する
		float angle = kAngleUnit * i;
		// Z軸周り回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		// 基本ベクトル回転させて速度ベクトルを得る
		velocity = TransformVector3(velocity, matrixRotation);
		// 移動処理
		worldTransforms_[i].translation_.x += velocity.x;
		worldTransforms_[i].translation_.y += velocity.y;
		worldTransforms_[i].translation_.z += velocity.z;
	}
	counter_ += 1.0f / 60.0f;
	if (counter_ >= kDuration) {
		counter_ = kDuration;

		isFinished_ = true;
	}
	if (isFinished_) {
		return;
	}
	

	for (auto& worldTransform : worldTransforms_) {

		worldTransform.UpdateMatrix();
		worldTransform.TransferMatrix();
	}
	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
	objectColor_.SetColor(color_);
	objectColor_.TransferMatrix();
}

void DeathParticles::Draw() {
	if (isFinished_) {
		return;
	}

	for (auto& worldTransform : worldTransforms_) {

		model_->Draw(worldTransform, *viewProjection_, &objectColor_);
	}
}
