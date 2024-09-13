#pragma once

#include "Audio.h"
#include "CameraController.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "MapChipField.h"
#include "Model.h"
#include "MyMath.h"
#include "Player.h" 
#include "Skydome.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include"DeathParticles.h"
#include"Phase.h"
#include <vector>



/// <summary>
/// ゲームシーン
/// </summary>
class GameScene2 {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene2();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene2();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// ブロックの生成
	/// </summary>
	void GenerateBlokcs();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//フェーズ切り替え
	void ChangePhase();

	void ClearPhase();

	// ブロックの反転とプレイヤーの位置修正を行う関数
	void InvertBlockPositionsWithCentering();


	bool  GetIsFinished() const { return finished_; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	ViewProjection viewProjection_; // ビュープロジェクション　ゲーム中に共通で一つ

	// テクスチャハンドル
	uint32_t texturHandle_ = 0;

	//キーボードテクスチャ
	uint32_t keyHandle_ = 0;
	Sprite* keySprite_ = nullptr;

	//反転テクスチャ
	uint32_t invertHandle_ = 0;
	Sprite* invertSprite_ = nullptr;

	//// サウンドデータハンドル
	//uint32_t soundDataHandle_ = 0;

	//// 音声再生ハンドル
	//uint32_t voiceHandle_ = 0;
	uint32_t BGMHandle_ = 0;
	uint32_t JumpSEHandle_ = 0;
	uint32_t InvertSEHandle_ = 0;

	// Player
	Model* model_ = nullptr;   // 3Dモデル
	Player* player_ = nullptr; // 自機

	// MapBlock
	Model* blockModel_ = nullptr;

	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	// SkyDome
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// MapChipField
	MapChipField* mapChipField_;

	// CameraController
	CameraController* cameraController_ = nullptr;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	//死エフェクト
	DeathParticles* deathParticles_ = nullptr;
	Model* deathParticlesModel_ = nullptr;

	//フェーズ
	Phase phase_;

	bool finished_ = false;
	bool invertFlg = true;
};