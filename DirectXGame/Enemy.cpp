#include "Enemy.h"


void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);
	model_ = model;
	// texthureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	viewProjection_ = viewProjection;
	velocity_ = {-kWalkSpeed, 0, 0}; // 速度
	walkTimer_ = 0.0f;

}

void Enemy::Update() {

	walkTimer_ += 1.0f / 60.0f;

	float param = std::sinf(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = fLerp(kWalkMotionAngleStart, kWalkMotionAngleEnd, radian);
	

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// 位置が一定の範囲を超えたらリセット
	const float resetPositionX = 1.0f;  // リセットする位置
	const float speedIncreaseFactor = 1.5f; // 速度の増加率
	const float maxSpeed = -0.2f;

	if (worldTransform_.translation_.x < resetPositionX) {
		worldTransform_.translation_.x = 17.0f; // リセット後のX座標
		if (velocity_.x > maxSpeed) {
			velocity_.x *= speedIncreaseFactor; // 速度を増加させる
			if (velocity_.x < maxSpeed) {
				velocity_.x = maxSpeed; // 速度が最大値を超えないようにする
			}
		}
	}

	worldTransform_.UpdateMatrix();
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *viewProjection_); }

Vector3 Enemy::GetWorldPosition() {

	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kEnemyWidth / 2.0f, worldPos.y - kEnemyHeight / 2.0f, worldPos.z - kEnemyWidth / 2.0f};
	aabb.max = {worldPos.x + kEnemyWidth / 2.0f, worldPos.y + kEnemyHeight / 2.0f, worldPos.z + kEnemyWidth / 2.0f};

	return aabb;
}

void Enemy::OnCollision(const Player* player) {

	(void)player;
}
