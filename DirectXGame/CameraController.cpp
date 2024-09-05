#define NOMINMAX
#include "CameraController.h"
#include "Player.h"

void CameraController::Initialize() { viewProjection_.Initialize(); }

void CameraController::Update() {
	// 追尾対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標系を計算
	const Vector3 targetVelocity = target_->GetVelocity();
	targetCameraPosition = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;
	viewProjection_.translation_ = Lerp(viewProjection_.translation_, targetCameraPosition, kInterpolationRate);

	viewProjection_.translation_.x = std::max(viewProjection_.translation_.x, movebleArea_.left);
	viewProjection_.translation_.x = std::min(viewProjection_.translation_.x, movebleArea_.right);
	viewProjection_.translation_.y = std::max(viewProjection_.translation_.y, movebleArea_.bottom);
	viewProjection_.translation_.y = std::min(viewProjection_.translation_.y, movebleArea_.top);

	viewProjection_.translation_.x = std::max(viewProjection_.translation_.x, targetWorldTransform.translation_.x + mergeArea.left);
	viewProjection_.translation_.x = std::min(viewProjection_.translation_.x, targetWorldTransform.translation_.x + mergeArea.right);
	viewProjection_.translation_.y = std::max(viewProjection_.translation_.y, targetWorldTransform.translation_.y + mergeArea.bottom);
	viewProjection_.translation_.y = std::min(viewProjection_.translation_.y, targetWorldTransform.translation_.y + mergeArea.top);

	// 行列を更新する
	viewProjection_.UpdateMatrix();
}

void CameraController::Draw() {}

void CameraController::Reset() {
	// 追尾対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標系を計算
	viewProjection_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}
