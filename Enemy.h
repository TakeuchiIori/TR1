#pragma once
#include "EnemyBullet.h"
#include "Input.h"
#include <list>
using namespace std;


// 行動フェーズ
enum class Phase {
	Approch, // 接近する
	Leave,   // 離脱する
};
class Player;
class Enemy {
public:
	
	/// <summary>
	/// コンストクラタ
	/// </summary>
	Enemy();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model);

	/// <summary>
	/// 接近処理の初期化
	/// </summary>
	void InitApproch();

	/// <summary>
	/// 接近処理
	/// </summary>
	void Approch();

	/// <summary>
	/// 接近処理の初期化
	/// </summary>
	void InitLeave();
	/// <summary>
	/// 離脱処理
	/// </summary>
	void Leave();

	/// <summary>
	/// 弾発射
	/// </summary>
	void Fire();

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

	void SetPlayer(Player* player) { player_ = player; }

	// 弾リスト取得
	const list<EnemyBullet*>& GetBullets() const { return bullets_; }

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	void TakeDamage(int damage);
public: // アクセッサ
	void SetHealth(int32_t health);
	void SetAttackPower(int32_t power);
	void SetMovementSpeed(float speed);
	void SetAttackFrequency(float frequency);
	void SetAILevel(int32_t level);
	
	int GetBaseHealth() const { return baseHealth_; }
	int GetHealth() { return health_; }
	int GetAttackPower() { return attackPower_; }
	int SetEnemyHrealth(int health) { return this->health_ = health; }
	int32_t GetBaseAttackPower() const;
	int32_t GetEnemyDeathTimer() { return isAlive_; }
	
	
	float Setradius() { return rad; }
	float GetHitEnemy() { return hitEnemy; }
	
	
	bool isDraw_ = true;
private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャ
	uint32_t textureHandle_ = 0;
	// フェーズ
	Phase phase_ = Phase::Approch;
	// 弾
	list<EnemyBullet*> bullets_;
	// 自キャラ
	Player* player_ = nullptr;
	
	Vector3 kApprochSpeed = {0.0f, 0.0f, -0.05f};
	Vector3 kLeaveSpeed = {0.0f, 0.0f, 0.0f}; //{-0.1f, 0.1f, 0.0f};
	// 半径
	float rad = 1.0f;
	float hitEnemy = 0.0f;
	// 敵のステータス
	//int health_ = 100;
	//int attackPower_ = 10;
	//const int baseHealth_ = 100;
	//const int baseAttackPower_ = 10;

	int32_t baseAttackPower_;
	int32_t attackPower_;
	float movementSpeed_;
	float attackFrequency_;
	int32_t baseHealth_;
	int32_t health_;
	int32_t aiLevel_;
	
	int32_t isAlive_ = 0;
	
private: //メンバ関数ポインタ
	// 発射タイマー
	int32_t shotTImer_ = 0;
	// 発射感覚
	int32_t kFireInterval = 60;

	static void (Enemy::*phaseEnemy[])();
	
};
