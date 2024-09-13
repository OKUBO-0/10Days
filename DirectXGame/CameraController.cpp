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

        // 追従対象が画面外に出ないように補正（X軸のみ）
        viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x,
            targetWorldTransform.translation_.x + margin.left,
            targetWorldTransform.translation_.x + margin.right);

        // 移動範囲制限（X軸のみ）
        viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x,
            movableArea_.left, movableArea_.right);
    }

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

void CameraController::StartRotation() {
    if (!isRotating_) {
        isRotating_ = true;
        rotationTimer_ = 0.0f;
        initialAngle_ = std::atan2(viewProjection_.translation_.z - target_->GetWorldPosition().z,
            viewProjection_.translation_.x - target_->GetWorldPosition().x);
    }
}

void CameraController::UpdateRotation() {
    if (isRotating_) {
        rotationTimer_ += 1.0f / 60.0f; // 1フレームあたりの時間
        float t = rotationTimer_ / kRotationDuration;

        if (t >= 1.0f) {
            isRotating_ = false;
            t = 1.0f;
        }

        // float型の定数を使用
        const float PI = 3.1415927f;
        float currentAngle = initialAngle_ + PI * t; // 180度回転

        // カメラとターゲットの位置のオフセット計算
        float deltaX = viewProjection_.translation_.x - target_->GetWorldPosition().x;
        float deltaZ = viewProjection_.translation_.z - target_->GetWorldPosition().z;
        float radius = static_cast<float>(std::sqrt(static_cast<double>(deltaX * deltaX + deltaZ * deltaZ)));

        // カメラ位置の更新 (円軌道上での移動)
        viewProjection_.translation_.x = target_->GetWorldPosition().x + std::cos(currentAngle) * radius;
        viewProjection_.translation_.z = target_->GetWorldPosition().z + std::sin(currentAngle) * radius;

        // カメラの傾きを180度回転 (Z軸を基準に回転)
        Vector3 zAxis = { 0.0f, 0.0f, 1.0f };
        Quaternion rotationQuat = Quaternion::FromAxisAngle(zAxis, PI); // Z軸周りで180度回転

        // カメラの回転行列に適用
        viewProjection_.rotation_ = rotationQuat * viewProjection_.rotation_;
    }
}