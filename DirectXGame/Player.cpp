#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include <DebugText.h>

float Player::kGravityAccleration = 0.05f;  // 静的メンバー変数の初期化


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

	/*if (Input::GetInstance()->PushKey(DIK_1)) {
		Clear();
	}*/
}

void Player::Draw() {
	model_->Draw(worldTransform_, *viewProjection_);
}

void Player::PrayerMove() {
	if (onGround_) {

		// 移動入力
		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {
			// 左右加速
			Vector3 accceleration = {};
			if (Input::GetInstance()->PushKey(DIK_D)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDirecion::kright) {
					lrDirection_ = LRDirecion::kright;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}
				accceleration.x += kAccleration * 2.0f;  // 移動速度を上げるための加速度を増やす
			}
			else if (Input::GetInstance()->PushKey(DIK_A)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDirecion::kLeft) {
					lrDirection_ = LRDirecion::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}
				accceleration.x -= kAccleration * 2.0f;  // 移動速度を上げるための加速度を増やす
			}

			velocity_.x += accceleration.x;
			velocity_.y += accceleration.y;
			velocity_.z += accceleration.z;

			// 最大速度を制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed * 1.5f, kLimitRunSpeed * 1.5f);

		}
		else {
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y *= (1.0f - kAttenuation);
			velocity_.z *= (1.0f - kAttenuation);
		}

		// ジャンプ処理（ジャンプの威力を半分に設定）
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			velocity_.x += 0;

			// 重力が反転している場合、下にジャンプ
			if (Player::kGravityAccleration < 0.0f) {
				velocity_.y -= kJampAcceleration * 0.5f;  // ジャンプ威力を半分に
			}
			else {
				velocity_.y += kJampAcceleration * 0.5f;  // ジャンプ威力を半分に
			}

			velocity_.z += 0;
		}

	}
	else {
		// 落下速度
		velocity_.x += 0;
		velocity_.y += -kGravityAccleration;  // 重力に応じて落下
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
			std::numbers::pi_v<float> *3.0f / 2.0f,
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
	// 天井に当たっているかどうかの判定
	if (info.ceiling) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling as ground\n");
		velocity_.y = 0.0f;

		// 天井を地面として扱う（反転時）
		if (Player::kGravityAccleration < 0.0f) {
			onGround_ = true;  // 逆さまのとき、天井をonGroundとして扱う
		}
	}
}

void Player::OnGroundSwitching(const CollisionMapInfo& info) {
	// 通常の重力状態（地面に着地した場合）
	if (Player::kGravityAccleration > 0.0f) {
		// プレイヤーが地面に着いているか判定
		if (info.landing) {
			DebugText::GetInstance()->ConsolePrintf("hit ground\n");
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
		else {
			onGround_ = false;
		}
	}
	else {
		// 逆さまの重力状態（天井にぶつかった場合）
		if (info.ceiling) {
			DebugText::GetInstance()->ConsolePrintf("hit ceiling as ground\n");
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
			onGround_ = true;  // 逆さまの場合、天井をonGroundとして扱う
		}
		else {
			onGround_ = false;
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

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock2) {
		hit = true;
	}
	// 右点の判定
	//   左点の判定

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock2) {
		hit = true;
	}

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock2) {
		hit = true;
	}
	// 右点の判定
	//   左点の判定

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock2) {
		hit = true;
	}

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDoor) {
		doorHit = true;
	}
	// 右点の判定
	// 左点の判定

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDoor) {
		doorHit = true;
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

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock2) {
		hit = true;
	}
	// 右点の判定
	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock2) {
		hit = true;
	}

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDoor) {
		doorHit = true;
	}
	// 右点の判定
	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDoor) {
		doorHit = true;
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

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightTop] + Vector3(+kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock2) {
		hit = true;
	}

	// 右下点の判定

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(+kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock2) {
		hit = true;
	}

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightTop] + Vector3(+kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDoor) {
		doorHit = true;
	}

	// 右下点の判定

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(+kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDoor) {
		doorHit = true;
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

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop] + Vector3(-kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock2) {
		hit = true;
	}

	// hidari下点の判定

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(-kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock2) {
		hit = true;
	}

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop] + Vector3(-kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDoor) {
		doorHit = true;
	}

	// hidari下点の判定

	indexSet = mapChipFild_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(-kCollisionsmallnumber, 0, 0));
	mapChipType = mapChipFild_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kDoor) {
		doorHit = true;
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

	return worldTransform_.translation_;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = { worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f };
	aabb.max = { worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f };

	return aabb;
}

void Player::SetWorldPosition(const Vector3& newPosition)
{
	position_ = newPosition;
	worldTransform_.translation_ = newPosition;  // ワールドトランスフォームの位置も更新
	worldTransform_.UpdateMatrix();  // 行列を更新して反映
}

// プレイヤーの回転を設定するメソッド
void Player::SetRotation(const Quaternion& rotation) {
	rotation_ = rotation;
	worldTransform_.rotation_ = QuaternionToEuler(rotation_); // クォータニオンからオイラー角に変換
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

// プレイヤーの重力方向を設定するメソッド
void Player::SetGravityDirection(const Vector3& gravityDirection) {
	gravityDirection_ = gravityDirection;
}

float Player::EaseOutSine(float x) { return cosf((x * std::numbers::pi_v<float>) / 2); }