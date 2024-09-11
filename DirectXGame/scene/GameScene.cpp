#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete blockModel_;
	delete debugCamera_;
	delete skydome_;
	delete mapChipField_;
	delete deathParticles_;
	delete deathParticlesModel_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// テクスチャ読み込み
	texturHandle_ = TextureManager::Load("pralyer.png");

	//// サウンドデータの読み込み
	//soundDataHandle_ = audio_->LoadWave("st005.wav");

	//// 音声再生
	//audio_->PlayWave(soundDataHandle_);
	//voiceHandle_ = audio_->PlayWave(soundDataHandle_, false);

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// SkyDome
	skydome_ = new Skydome();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_->Initialize(modelSkydome_, &viewProjection_);

	// Block
	blockModel_ = Model::CreateFromOBJ("block", true);

	// DebugCamera
	debugCamera_ = new DebugCamera(1280, 720);

	// MapChipFiled
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");
	GenerateBlokcs();

	// Player
	player_ = new Player();
	model_ = Model::CreateFromOBJ("player", true); // 3Dモデルの生成
	Vector3 playerPostion = mapChipField_->GetMapChipPostionByIndex(1, 34);
	player_->SetMapChipField(mapChipField_);
	player_->Initialize(model_, &viewProjection_, playerPostion);

	// CameraController
	CameraController::Rect cameraArea = { 11.0f, 28.0f, 6.0f, 48.0f };
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->SetMovableArea(cameraArea);
	cameraController_->Reset();

	// DeathParticles
	deathParticles_ = new DeathParticles;
	deathParticlesModel_ = Model::CreateFromOBJ("deathParticle", true); // 3Dモデルの生成
	deathParticles_->Initialize(playerPostion, deathParticlesModel_, &viewProjection_);

	// phase
	phase_ = Phase::kplay;
}

void GameScene::Update() {

	ChangePhase();

	switch (phase_) {

	case Phase::kplay:
		break;

	case Phase::kDeath:
		break;
	}

	player_->Update();

	if (player_->GetIsDead_() == true) {
		deathParticles_->Update();
	}

	if (player_->GetIsDead_() == false) {
		cameraController_->Update();
	}

	/*for (Enemy* enemy : enemies_) {
		if (!nullptr) {
			enemy->Update();
		}
	}*/

	// Block
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_C)) {

		isDebugCameraActive_ = true;
	}

#endif // DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列
		viewProjection_.TransferMatrix();

	}
	else {
		viewProjection_.matView = cameraController_->GetViewProjection().matView;
		viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の更新と転送
		viewProjection_.TransferMatrix();
	}


	//反転処理
	if (input_->TriggerKey(DIK_DOWN)) {
		mapChipField_->InvertMap();
		InvertBlockPositionsWithCentering();  // 位置を調整しながら反転する
		cameraController_->StartRotation();  // カメラの回転を開始
	}





}

void GameScene::GenerateBlokcs() {
	// 要素数
	uint32_t numBlokVirtical = mapChipField_->GetNumBlockVirtical();     // 縦
	uint32_t numBlokHorizontal = mapChipField_->GetNumBlockHorizontal(); // 横

	// 要素数を変更する
	// 列数を設定
	worldTransformBlocks_.resize(numBlokVirtical);
	for (uint32_t i = 0; i < numBlokVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlokHorizontal);
	}

	// キューブ生成
	for (uint32_t i = 0; i < numBlokVirtical; ++i) {
		for (uint32_t j = 0; j < numBlokHorizontal; ++j) {
			// マップチップの種類を取得
			MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(j, i);

			// 1（ブロック）の場合のみ描画
			if (mapChipType == MapChipType::kBlock) {
				// 既存のワールドトランスフォームがない場合は新たに生成
				if (!worldTransformBlocks_[i][j]) {
					WorldTransform* worldTransform = new WorldTransform();
					worldTransform->Initialize();
					worldTransformBlocks_[i][j] = worldTransform;
				}
				// ブロックの位置を設定
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPostionByIndex(j, i);
				worldTransformBlocks_[i][j]->matWorld_ = MakeAffineMatrix(
					worldTransformBlocks_[i][j]->scale_,
					worldTransformBlocks_[i][j]->rotation_,
					worldTransformBlocks_[i][j]->translation_);
				worldTransformBlocks_[i][j]->TransferMatrix();
			}
			else {
				// 0（空白）の場合、ワールドトランスフォームを削除（描画しない）
				if (worldTransformBlocks_[i][j]) {
					delete worldTransformBlocks_[i][j];
					worldTransformBlocks_[i][j] = nullptr;
				}
			}
		}
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	if (player_->GetIsDead_() == false) {
		player_->Draw();
	}

	/*for (Enemy* enemy : enemies_) {
		if (!nullptr) {
			enemy->Draw();
		}
	}*/

	if (player_->GetIsDead_() == true) {
		deathParticles_->Draw();
	}

	skydome_->Draw();
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			blockModel_->Draw(*worldTransformBlock, viewProjection_);
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::ChangePhase() {

	switch (phase_) {

	case Phase::kplay:

		if (player_->GetIsDead_() == true) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			deathParticles_->Initialize(deathParticlesPosition, deathParticlesModel_, &viewProjection_);
		}

		break;

	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->GetIsFinished()) {
			finished_ = true;
		}
		break;
	}
}

void GameScene::InvertBlockPositionsWithCentering() {
	// マップの縦横のブロック数を取得
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVirtical();     // 縦
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal(); // 横

	// 一時的な2D配列を作成してマップデータを保存
	std::vector<std::vector<MapChipType>> tempMap(numBlockVertical, std::vector<MapChipType>(numBlockHorizontal));

	// 現在のマップデータを一時配列にコピー
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			tempMap[i][j] = mapChipField_->GetMapChipTypeByIndex(j, i);
		}
	}

	// マップ全体をループして処理（上下左右反転）
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			uint32_t invertedI = numBlockVertical - 1 - i;
			uint32_t invertedJ = numBlockHorizontal - 1 - j;

			MapChipType currentChip = tempMap[i][j];

			// block2 の場合は反転しない
			if (currentChip == MapChipType::kBlock2) {
				mapChipField_->SetMapChipTypeByIndex(invertedJ, invertedI, currentChip);
				continue;
			}

			// 1と0の入れ替え（ブロックと空白の反転）
			MapChipType invertedChip = (currentChip == MapChipType::kBlock) ? MapChipType::kBlank : MapChipType::kBlock;

			// マップチップを更新（位置を反転させて）
			mapChipField_->SetMapChipTypeByIndex(invertedJ, invertedI, invertedChip);

			if (invertedChip == MapChipType::kBlock) {
				// ブロックが存在しなければ新しいワールドトランスフォームを作成
				if (!worldTransformBlocks_[invertedI][invertedJ]) {
					WorldTransform* worldTransform = new WorldTransform();
					worldTransform->Initialize();
					worldTransformBlocks_[invertedI][invertedJ] = worldTransform;
				}
				// ブロックの位置を設定
				Vector3 newPosition = mapChipField_->GetMapChipPostionByIndex(invertedJ, invertedI);
				worldTransformBlocks_[invertedI][invertedJ]->translation_ = newPosition;
				worldTransformBlocks_[invertedI][invertedJ]->matWorld_ = MakeAffineMatrix(
					worldTransformBlocks_[invertedI][invertedJ]->scale_,
					worldTransformBlocks_[invertedI][invertedJ]->rotation_,
					worldTransformBlocks_[invertedI][invertedJ]->translation_);
				worldTransformBlocks_[invertedI][invertedJ]->TransferMatrix();
			}
			else {
				// ブロックが空白になる場合は、メモリを解放して削除
				if (worldTransformBlocks_[invertedI][invertedJ]) {
					delete worldTransformBlocks_[invertedI][invertedJ];
					worldTransformBlocks_[invertedI][invertedJ] = nullptr;
				}
			}
		}
	}


	// プレイヤーの位置を保持
	Vector3 playerPositionBeforeRotation = player_->GetWorldPosition();

	// プレイヤーがすでに逆さまかどうかをチェックし、逆さまなら回転を行わない
	Vector3 xAxis = Vector3(1.0f, 0.0f, 0.0f);
	float angleRad = 3.14159f; // 180度

	// プレイヤーの回転を確認する
	Quaternion currentRotation = player_->GetRotation();
	Quaternion invertedRotation = Quaternion::FromAxisAngle(xAxis, angleRad);

	// 逆さまの状態かどうかをチェック（180度回転しているか）
	bool isInverted = (currentRotation == invertedRotation);

	if (!isInverted) {
		// プレイヤーを逆さまにするためにX軸方向に180度回転
		player_->SetRotation(invertedRotation);
	}
	else {
		// 180度回転を元に戻す
		player_->SetRotation(Quaternion::Identity());
	}

	// プレイヤーの位置を回転前の位置に戻す
	player_->SetWorldPosition(playerPositionBeforeRotation);

	// 重力の反転
	Player::kGravityAccleration = -Player::kGravityAccleration;

	// プレイヤーがブロックにめり込まないようにY軸方向の調整
	Vector3 newPlayerPosition = player_->GetWorldPosition();
	if (Player::kGravityAccleration > 0.0f) {
		newPlayerPosition.y += 1.0f; // 通常の重力方向時
	}
	else {
		newPlayerPosition.y -= 1.0f; // 逆さまの重力時
	}

	// プレイヤーの位置を更新
	player_->SetWorldPosition(newPlayerPosition);
}