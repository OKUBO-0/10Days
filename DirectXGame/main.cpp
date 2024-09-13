#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "GameScene2.h"
#include "GameScene3.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "TitleScene.h"
#include "WinApp.h"

GameScene* gameScene = nullptr;
GameScene2* gameScene2 = nullptr;
GameScene3* gameScene3 = nullptr;
TitleScene* titeleScene = nullptr;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
	kGame2,
	kGame3,

};
Scene scene = Scene::kUnknown;

void ChengeScene() {

	switch (scene) {
	case Scene::kTitle:
		if (titeleScene->GetISFinished()) {
			// sceneの変更
			scene = Scene::kGame;
			// 旧シーンかいほう
			delete titeleScene;
			titeleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->GetIsFinished()) {
			// sceneの変更
			scene = Scene::kGame2;
			// 旧シーンかいほう
			delete gameScene;
			titeleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene2 = new GameScene2;
			gameScene2->Initialize();
		}
		break;
	case Scene::kGame2:
		if (gameScene2->GetIsFinished()) {
			// sceneの変更
			scene = Scene::kGame3;
			// 旧シーンかいほう
			delete titeleScene;
			titeleScene = nullptr;
			// 新シーンの生成と初期化
	
			gameScene3 = new GameScene3;
			gameScene3->Initialize();
		}
		break;

	case Scene::kGame3:
		if (gameScene3->GetIsFinished()) {
			// sceneの変更
			scene = Scene::kTitle;
			// 旧シーンかいほう
			delete titeleScene;
			titeleScene = nullptr;
			// 新シーンの生成と初期化
			titeleScene = new TitleScene;
			titeleScene->Initialize();
		}
		break;;
	}
}

void UpdateScene() {

	switch (scene) {
	case Scene::kTitle:
		titeleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kGame2:
		gameScene2->Update();
		break;
	case Scene::kGame3:
		gameScene3->Update();
		break;
	}
}

void DrawScene() {

	switch (scene) {
	case Scene::kTitle:
		titeleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kGame2:
		gameScene2->Draw();
		break;

	case Scene::kGame3:
		gameScene3->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"10days");

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	scene = Scene::kTitle;
	titeleScene = new TitleScene();
	titeleScene->Initialize();

	//// ゲームシーンの初期化
	// gameScene = new GameScene();
	// gameScene->Initialize();

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();
		//// ゲームシーンの毎フレーム処理
		// gameScene->Update();
		// タイトル
		ChengeScene();

		UpdateScene();
		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();
		//// ゲームシーンの描画
		// gameScene->Draw();
		// タイトル
		DrawScene();
		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 各種解放
	delete gameScene3;
	delete gameScene2;
	delete gameScene;
	delete titeleScene;
	// 3Dモデル解放
	Model::StaticFinalize();
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();


	return 0;
}