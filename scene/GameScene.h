#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 衝突判定を応答
	/// </summary>
	void CheackAllCollisions();

	/// <summary>
	/// 敵の難易度調整する関数
	/// </summary>
	void AdjustEnemyAI();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	Vector3 calculateEnemySpeed(const Vector3& playerPos, const Vector3& enemyPos);

	float weightedSum(Player* player, Enemy* enemy, const std::vector<float>& weights);
	float weightedSumAndAverage(Player* player, Enemy* enemy, const std::vector<float>& weights);
private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	// テクスチャ
	uint32_t textureHandle_ = 0;
	// 3Dモデル
	Model* model_ = nullptr;
	// 天球モデル
	Model* modelSkydome_ = nullptr;
	// 自機モデル
	Model* modelPlayer_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	bool isDebugCameraActive_ = false;
	// 自キャラ
	Player* player_ = nullptr;
	// 敵キャラ
	Enemy* enemy_ = nullptr;
	// 天球
	Skydome* skydome_ = nullptr;

	float enemySpawnRate_;
	float powerUpSpawnRate_;
	int32_t level;
	float pressureLine;
	float recoveryLine;
	float average_;
};
