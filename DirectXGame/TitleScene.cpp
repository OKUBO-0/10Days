#include "TitleScene.h"
#include <numbers>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
	delete model_;
	delete stage1model_;
	delete stage2model_;
	delete stage3model_;
	delete skydome_;
}

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	model_ = Model::CreateFromOBJ("title", true);
	stage1model_ = Model::CreateFromOBJ("stage1", true);
	stage2model_ = Model::CreateFromOBJ("stage2", true);
	stage3model_ = Model::CreateFromOBJ("stage3", true);
	worldTransform_.Initialize();
	viewProjection_.Initialize();

	// SkyDome
	skydome_ = new Skydome();
	modelSkydome_ = Model::CreateFromOBJ("skydomeTitle", true);
	skydome_->Initialize(modelSkydome_, &viewProjection_);

	Timer_ = 0.0f;

	// タイトルを近づける
	worldTransform_.translation_ = { 0.0f, 1.0f, 0.0f };  // z値を調整して近づける
}

void TitleScene::Update() {

	// マウス座標を取得
	Vector2 mousePos = input_->GetMousePosition();
#ifdef DEBUG



	// ImGuiでマウス座標を表示
	ImGui::Begin("Mouse Position");
	ImGui::Text("Mouse X: %.1f", mousePos.x);  // X座標を表示
	ImGui::Text("Mouse Y: %.1f", mousePos.y);  // Y座標を表示
	ImGui::End();
#endif // DEBUG
	// マウスクリック判定（前の例と同じ処理）
	int clickArea1X = 410;
	int clickArea1Y = 490;

	int clickArea2X = 590;
	int clickArea2Y = 490;

	int clickArea3X = 770;
	int clickArea3Y = 490;

	int clickAreaWidth = 100;
	int clickAreaHeight = 100;

	if (mousePos.x >= clickArea1X && mousePos.x <= clickArea1X + clickAreaWidth &&
		mousePos.y >= clickArea1Y && mousePos.y <= clickArea1Y + clickAreaHeight) {
		if (input_->IsTriggerMouse(0)) {
			finished_ = true;
		}
	}

	if (mousePos.x >= clickArea2X && mousePos.x <= clickArea2X + clickAreaWidth &&
		mousePos.y >= clickArea2Y && mousePos.y <= clickArea2Y + clickAreaHeight) {
		if (input_->IsTriggerMouse(0)) {
			finished_ = true;
		}
	}

	if (mousePos.x >= clickArea3X && mousePos.x <= clickArea3X + clickAreaWidth &&
		mousePos.y >= clickArea3Y && mousePos.y <= clickArea3Y + clickAreaHeight) {
		if (input_->IsTriggerMouse(0)) {
			finished_ = true;
		}
	}

	//if (Input::GetInstance()->PushKey(DIK_SPACE)) {

	//	finished_ = true;

	//}

	Timer_ += 1.0f / 60.0f;
	float param = std::sin(2.0f * std::numbers::pi_v<float> *Timer_ / kWalklMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.y = radian * (std::numbers::pi_v<float> / 90.0f);
	// 行列計算
	worldTransform_.UpdateMatrix();

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 定数バッファに転送する
	worldTransform_.TransferMatrix();


}

void TitleScene::Draw() {

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


	model_->Draw(worldTransform_, viewProjection_);
	stage1model_->Draw(worldTransform_, viewProjection_);
	stage2model_->Draw(worldTransform_, viewProjection_);
	stage3model_->Draw(worldTransform_, viewProjection_);

	skydome_->Draw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

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