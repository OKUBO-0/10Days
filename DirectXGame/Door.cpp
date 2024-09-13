#include "Door.h"

Door::Door() {}

Door::~Door() {
	delete model_;
}

void Door::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	viewProjection_ = viewProjection;
	worldTransform_.UpdateMatrix();
}

void Door::Update() {
	worldTransform_.UpdateMatrix();
}

void Door::Draw() {
	model_->Draw(worldTransform_, *viewProjection_);
}