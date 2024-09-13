#include <algorithm>
#include "CameraController.h"
#include "Player.h"
#include <format>
#include <cmath> // fmod関数を使用

// 線形補間関数
template <typename T>
T Lerp(const T& a, const T& b, float t) {
    return a * (1 - t) + b * t;
}

void CameraController::Initialize() {
    viewProjection_.Initialize();
}

void CameraController::Update() {

#ifdef DEBUG
	// ImGuiでカメラの回転状態を表示
	ImGui::Begin("CameraRotate");
	ImGui::Text("viewprojection.Z: %0.1f", viewProjection_.rotation_.z);  // Z軸回転角度を表示
	ImGui::End();
#endif

	// 追従対象のワールドトランスフォームを取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	if (isRotating_) {
		// 回転アニメーション中の処理
		UpdateRotation();
	}
	else {
		// 通常の追従処理

		// 追従対象の座標、移動速度とオフセットで目標座標を計算
		dest_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
		dest_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
		dest_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias;

		// X軸方向の補間率を設定
		const float kHorizontalInterpolationRate = kInterpolationRate * 0.5f; // 例: 通常の半分の速度

		// Y軸方向の補間率を設定
		const float kVerticalInterpolationRate = kInterpolationRate * 0.3f; // 例: 通常の30%の速度

		// 座標補間によりゆったり追従（X軸とY軸で異なる速度）
		viewProjection_.translation_.x = Lerp(viewProjection_.translation_.x, dest_.x, kHorizontalInterpolationRate);
		viewProjection_.translation_.y = Lerp(viewProjection_.translation_.y, dest_.y, kVerticalInterpolationRate);
		viewProjection_.translation_.z = Lerp(viewProjection_.translation_.z, dest_.z, kInterpolationRate);

		// 追従対象が画面外に出ないように補正（X軸）
		viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x,
			targetWorldTransform.translation_.x + margin.left,
			targetWorldTransform.translation_.x + margin.right);

		// 移動範囲制限（X軸）
		viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x,
			movableArea_.left, movableArea_.right);

		// 追従対象が画面外に出ないように補正（Y軸）
		viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, 
			targetWorldTransform.translation_.y + margin.bottom, 
			targetWorldTransform.translation_.y + margin.top);

		// 移動範囲制限（X軸）
		viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, 
			movableArea_.bottom, movableArea_.top);
	}

	// ビュープロジェクション行列の更新
	viewProjection_.UpdateViewMatrixZFree();
}

void CameraController::Reset() {
	// 必要であればカメラの位置をリセットする処理を追加
}

void CameraController::UpdateRotation() {
	if (isRotating_) {
		rotationTimer_ += 1.0f / 30.0f; // 1フレームあたりの時間（60FPS想定）
		float t = rotationTimer_ / kRotationDuration;
		if (t >= 1.0f) {
			isRotating_ = false;
			t = 1.0f;
		}
		// 回転のための定数
		const float PI = 3.1415927f;
		// プレイヤーの位置を取得
		Vector3 playerPos = target_->GetWorldPosition();
		viewProjection_.translation_.y = playerPos.y + 5.0f; // カメラの高さ調整

		// 反転時は180度回転
		if (t < 1.5f) {
			viewProjection_.rotation_.z = PI * (2.0f * t);
		}
		else {
			viewProjection_.rotation_.z = PI * (2.0f * (1.0f - t));
		}

		// 新しく作った UpdateViewMatrixZFree を呼び出してカメラの行列を更新
		viewProjection_.UpdateViewMatrixZFree();
	}
}

void CameraController::StartRotation() {

	if (!isRotating_) {
		isRotating_ = true;
		rotationTimer_ = 0.0f;

		// 初期角度を取得
		/*initialAngle_ = std::atan2(viewProjection_.translation_.z - target_->GetWorldPosition().z,
			viewProjection_.translation_.x - target_->GetWorldPosition().x);*/
	}
}