#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include "Input.h"
#include "MathFunc.h"
#include "imgui.h"
#include "PlayerBullet.h"
#include <numbers>
#include <cassert>
#include <list>
#include <chrono>
#include <ctime>
using namespace std;
using namespace std::numbers;
class Enemy;
class Player {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Player();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Player();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(Model* model, uint32_t textureHandle);

    /// <summary>
    /// 旋回
    /// </summary>
    void Rotate();

    /// <summary>
    /// 敵のダメージをもらう
    /// </summary>
    void TakeDamage(int damage);

    /// <summary>
    /// 回復タイマー
    /// </summary>
    void Heal();

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

    /// <summary>
    /// ワールド座標を取得
    /// </summary>
    Vector3 GetWroldPosition();

    /// <summary>
    /// 弾リスト取得
    /// </summary>
    const list<PlayerBullet*>& GetBullets() const { return bullets_; }

    /// <summary>
    /// 発射間隔を取得
    /// </summary>
    float GetShootInterval() const { return shootInterval_; }

public: // アクセッサ
    void SetPlayer(Enemy* enemy) { enemy_ = enemy; }
    int GetHealth() const { return health_; }
    void SetHealth(int health) { this->health_ = health; }
    void SetAttackPower(int attackPower) { this->attackPower_ = attackPower; }
    int GetBaseHealth() const { return baseHealth_; }
    int GetBaseAttackPower() const { return baseAttackPower_; }
    int32_t GetHealth() { return health_; }
    int GetAttackPower() { return attackPower_; }
    float Setradius() { return rad; }
    float GetSpeed() { return kCharacterSpeed; }
    float GettimeUntilNextShoot() { return timeUntilNextShoot; }
    bool GetisHeal() { return heal; }
    bool GetisPenaltyActive_() { return isPenaltyActive_; }
private: // メンバ関数
    /// <summary>
    /// 攻撃
    /// </summary>
    void Attack();

private: // メンバ変数
    // ワールドトランスフォーム
    WorldTransform worldTransform_;
    // モデル
    Model* model_ = nullptr;
    // テクスチャ
    uint32_t textureHandle_ = 0;
    // キーボード入力
    Input* input_ = nullptr;
    // 弾
    list<PlayerBullet*> bullets_;
    // 敵
    Enemy* enemy_ = nullptr;
    // 半径
    float rad = 1.0f;
    // プレイヤーのHPの初期値
    int32_t health_ = 100;
    std::chrono::seconds::rep lastDamageTime_;
    // プレイヤーのステータス
    int attackPower_ = 10;
    const int baseHealth_ = 100;
    const int baseAttackPower_ = 10;
    float HitPlayer = 0.0f;
    float kCharacterSpeed = 0.3f;
    float healCooldown_ = 5.0f;
    float  healInterval_ = 1.0f;
    float timeUntilNextShoot;
    bool heal = false;
    bool isHealCounting_ = false;
    bool isPenaltyActive_ = false;
    // 発射間隔計測用の変数
    float shootInterval_ = 0.0f;
   time_t lastShootTime_  = 0;
    // 次の発射時刻
    time_t nextShootTime_ = 0; 
};
