#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "MathFunc.h"
#include <TextureManager.h>
#include <cassert>
class EnemyBullet {
public:
	/// <summary>
	/// コンストクラタ
	/// </summary>
	EnemyBullet();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyBullet();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision();

	/// <summary>
	/// デスタイマー関数
	/// </summary>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWaroldPosition();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

public:

	float Setradius() { return rad; }

private: // メンバ変数

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデルのポインタ
	Model* model_ = nullptr;
	// テクスチャ
	uint32_t textureHandle_ = 0;
	// 速度
	Vector3 velocity_;
	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 5;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;
	// 半径
	float rad = 1.0f;

};
