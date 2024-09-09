#include "Viewprojection.h"
#include "Player.h"
#include "Vector3.h"



class Player;

class CameraController {

public:

	// 矩形
	struct Rect {
		float left = 0.0f;// 左端
		float right = 1.0f;// 右端
		float bottom = 0.0f;// 下端
		float top = 1.0f;// 上端
	};

	void Initialize();

	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();


	void SetMovableArea(const Rect& area)
	{
		movableArea_ = area;

		//		movableArea_.left= area.left; 
		//		movableArea_.right = area.right;
		//		movableArea_.bottom = area.bottom;
		//		movableArea_.top = area.top;
	}

	ViewProjection GetViewPosition();

	const ViewProjection& GetViewProjection()const { return viewProjection_; }

private:
	// ビュープロジェクション
	ViewProjection viewProjection_;

	Player* target_ = nullptr;

	// 追従対象とカメラの座標の差（オフセット）
	Vector3 targetOffset_{ 0,0,-16.0f };

	Vector3 destination_;

	Vector3 dest_{ 0,0,-15.0f };

	// カメラ移動範囲
	Rect movableArea_ = { 0, 100, 0, 100 };

	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect margin = { -9.0f, 0.0f, -5.0f, 5.0f };

	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f;

	// 速度掛率
	static inline const float kVelocityBias = 15.0f;

};