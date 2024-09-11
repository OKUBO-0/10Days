#include <algorithm>
#include "CameraController.h"
#include "Player.h"

// 線形補間関数
template <typename T>
T Lerp(const T& a, const T& b, float t) {
    return a * (1 - t) + b * t;
}

void CameraController::Initialize()
{
    viewProjection_.Initialize();
}

void CameraController::Update() {
    // 追従対象のワールドトランスフォームを取得
    const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
    const Vector3& targetVelocity = target_->GetVelocity();

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

    // 移動範囲制限X軸
    viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x, targetWorldTransform.translation_.x + margin.left, targetWorldTransform.translation_.x + margin.right);
    viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x, movableArea_.left, movableArea_.right);

    // 移動範囲制限Y軸
    viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, targetWorldTransform.translation_.y + margin.bottom, targetWorldTransform.translation_.y + margin.top);
    viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, movableArea_.bottom, movableArea_.top);

    // ビュープロジェクション行列の更新
    viewProjection_.UpdateMatrix();
}

void CameraController::Reset()
{
    const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

    viewProjection_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
    viewProjection_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
    viewProjection_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}

ViewProjection CameraController::GetViewPosition()
{
    return ViewProjection();
}