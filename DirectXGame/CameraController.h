#pragma once
#include "ViewProjection.h"



class Player;
class CameraController {

public:
	struct Rect {

		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void Reset();

	const ViewProjection& GetViewProjection() const { return viewProjection_; }
	void SetTarget(Player* target) { target_ = target; }
	void SetMovableArea(Rect area) { movebleArea_ = area; }

private:
	ViewProjection viewProjection_;
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f};
	Rect movebleArea_ = {0, 100, 0, 100};
	Vector3 targetCameraPosition;//カメラの目標座標
	static inline const float kInterpolationRate = 0.5f;
	static inline const float kVelocityBias = 10.0f;
	Rect mergeArea = {
	    0.0f,
	    20.0f,
	    0.0f,
	    20.0f,
	};
	
};
