#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include <DebugText.h>

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);
	model_ = model;
	// texthureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	// worldTransform_.rotation_.y = 0;
	viewProjection_ = viewProjection;
}

void Player::Update() {

	PrayerMove();
	// 衝突判定を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;
	// マップ衝突チェック
	MapCollision(collisionMapInfo);
	// 移動
	CeilingCollisionMove(collisionMapInfo);
	PlayerCollisionMove(collisionMapInfo);
	OnGroundSwitching(collisionMapInfo);
	HitWallCollisionMove(collisionMapInfo);
	PrayerTurn();

	worldTransform_.UpdateMatrix();
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_); }

void Player::PrayerMove() {

	if (onGround_) {
		// 移動入力
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 左右加速
			Vector3 accceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				if (lrDirection_ != LRDirecion::kright) {
					lrDirection_ = LRDirecion::kright;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}

				accceleration.x += kAccleration;

			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDirecion::kLeft) {
					lrDirection_ = LRDirecion::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}

				accceleration.x -= kAccleration;
			}
			velocity_.x += accceleration.x;
			velocity_.y += accceleration.y;
			velocity_.z += accceleration.z;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {

			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y *= (1.0f - kAttenuation);
			velocity_.z *= (1.0f - kAttenuation);
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {

			velocity_.x += 0;
			velocity_.y += kJampAcceleration;
			velocity_.z += 0;
		}

	} else {
		// 落下速度
		velocity_.x += 0;
		velocity_.y += -kGravityAccleration;
		velocity_.z += 0;
		// 落下速度制限

		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::PrayerTurn() {
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;

		// 左右の角度テーブル
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,
		    std::numbers::pi_v<float> * 3.0f / 2.0f,
		};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = destinationRotationY * EaseOutSine(turnTimer_);
		;
		;
	}
}

void Player::MapCollision(CollisionMapInfo& info) {

	CollisionMapInfoTop(info);
	CollisionMapInfoBootm(info);
	CollisionMapInfoRight(info);
	CollisionMapInfoLeft(info);
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offseetTable[kNumCorner] = {

	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };

	return center + offseetTable[static_cast<uint32_t>(corner)];
}

void Player::PlayerCollisionMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_.x += info.move.x;
	worldTransform_.translation_.y += info.move.y;
	worldTransform_.translation_.z += info.move.z;
}

// 天井当たった？
void Player::CeilingCollisionMove(const CollisionMapInfo& info) {

	if (info.ceiling) {

		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0.0f;
	}
}

void Player::OnGroundSwitching(const CollisionMapInfo& info) {

	if (onGround_) {
		if (velocity_.y > 0.0f) {

			onGround_ = false;

		} else {
			// 移動後4つの計算
			std::array<Vector3, kNumCorner> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}
			MapChipType mapChipType;
			// 真下の当たり判定
			bool hit = false;

			// 左点の判定
			IndexSet indexSet;
			indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kCollisionsmallnumber, 0));
			mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右点の判定
			indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kCollisionsmallnumber, 0));
			mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {

				onGround_ = false;
			}
		}

	} else {

		if (info.landing) {
			DebugText::GetInstance()->ConsolePrintf("hit landing\n");
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}
}

void Player::HitWallCollisionMove(const CollisionMapInfo& info) {

	if (info.hitWall) {

		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::CollisionMapInfoTop(CollisionMapInfo& info) {

	if (info.move.y <= 0) {
		return;
	}
	// 移動後4つの計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定
	bool hit = false;
	// 左点の判定
	IndexSet indexSet;
	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右点の判定
	//   左点の判定

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// hit
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipFild_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipFild_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 天井に当たったらことを記録する
		info.ceiling = true;
	}
}

void Player::CollisionMapInfoBootm(CollisionMapInfo& info) {
	if (info.move.y >= 0) {
		return;
	}
	// 移動後4つの計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	// 真下の当たり判定
	bool hit = false;

	// 左点の判定
	IndexSet indexSet;
	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右点の判定
	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// hit
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipFild_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipFild_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::min(0.0f, rect.bottom - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		// 地面に当たったらことを記録する
		info.landing = true;
	}
}

void Player::CollisionMapInfoRight(CollisionMapInfo& info) {

	if (info.move.x <= 0) {
		return;
	}
	// 移動後4つの計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定
	bool hit = false;
	// 右上点の判定
	IndexSet indexSet;
	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightTop] + Vector3(+kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(+kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// hit
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		DebugText::GetInstance()->ConsolePrintf("hit hitwall\n");
		indexSet = mapChipFild_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth, 0 / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipFild_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
		// 壁に当たったらことを記録する
		info.hitWall = true;
	}
}

void Player::CollisionMapInfoLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0) {
		return;
	}
	// 移動後4つの計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定
	bool hit = false;
	// hidari上点の判定
	IndexSet indexSet;
	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop] + Vector3(-kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// hidari下点の判定

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(-kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// hit
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		DebugText::GetInstance()->ConsolePrintf("hit hitwall\n");
		indexSet = mapChipFild_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipFild_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.x = std::min(0.0f, rect.left - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
		// 壁に当たったらことを記録する
		info.hitWall = true;
	}
}

Vector3 Player::GetWorldPosition() {

	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {

	(void)enemy;
	//velocity_.y += kJampAcceleration;
	isDead_ = true;
}

float Player::EaseOutSine(float x) { return cosf((x * std::numbers::pi_v<float>) / 2); }
