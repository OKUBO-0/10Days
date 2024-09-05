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
	delete enemyModel_;
	delete deathParticles_;
	delete deathParticlesModel_;

	for (Enemy* enemy : enemies_) {

		delete enemy;
	}

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
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlokcs();

	// Player
	player_ = new Player();
	model_ = Model::CreateFromOBJ("player", true); // 3Dモデルの生成
	Vector3 playerPostion = mapChipField_->GetMapChipPostionByIndex(1, 18);
	player_->SetMapChipField(mapChipField_);
	player_->Initialize(model_, &viewProjection_, playerPostion);

	// Enemy
	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	for (int32_t i = 0; i < enemynumber; i++) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPostionByIndex(17 - i - i - i, 18 - i -i);
		newEnemy->Initialize(enemyModel_, &viewProjection_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	// CameraController
	CameraController::Rect cameraArea = {0.0f, 100 - 12.0f, 6.0f, 6.0f};
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

	for (Enemy* enemy : enemies_) {
		if (!nullptr) {
			enemy->Update();
		}
	}

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

	CheckAllCollisions();
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

	} else {
		viewProjection_.matView = cameraController_->GetViewProjection().matView;
		viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の更新と転送
		viewProjection_.TransferMatrix();
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

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {

				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPostionByIndex(j, i);
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

	for (Enemy* enemy : enemies_) {
		if (!nullptr) {
			enemy->Draw();
		}
	}

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

void GameScene::CheckAllCollisions() {

	AABB aabb1, aabb2;
	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {

		aabb2 = enemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {

			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
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