#include "Enemy.h"
#include "Player.h"

Enemy::Enemy() : baseAttackPower_(10), attackPower_(10), movementSpeed_(1.0f), attackFrequency_(1.0f), baseHealth_(100), health_(100), aiLevel_(1) {
	
 }

Enemy::~Enemy() { 
	// bullet_の解放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

 void(Enemy::*Enemy::phaseEnemy[])() = {
	 &Enemy::Approch,
     &Enemy::Leave

};

void Enemy::Initialize(Model* model) {
	// NILLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = TextureManager::Load("./Resources./Enemy./enemy.png");
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, -5.0f, 20.0f};
	health_ = 100;
	// 接近フェーズ
	InitApproch();
}
void Enemy::InitApproch() {
	// 発射タイマーを初期化
	shotTImer_ = 10;
}
void Enemy::Approch() {
	shotTImer_--;
	if (shotTImer_ == 0) {
		Fire();
		shotTImer_ = kFireInterval;
	}
	// 移動（ベクトルを加算）
	//worldTransform_.translation_ += kApprochSpeed;
	if (worldTransform_.translation_.z < 0.0f) {
		//phase_ = Phase::Leave;
	}
}

void Enemy::InitLeave() {
	// 発射タイマーを初期化
	shotTImer_ = 10;
}

void Enemy::Leave() {
	shotTImer_--;
	if (shotTImer_ == 0) {
		Fire();
		shotTImer_ = kFireInterval;
	}
	 //移動（ベクトルを加算）
	worldTransform_.translation_ += kLeaveSpeed;
	 if (worldTransform_.translation_.y > 20.0f) {
		worldTransform_.translation_ = {0.0f, -5.0f, 20.0f};
		phase_ = Phase::Approch;
	 }
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得　（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
void Enemy::TakeDamage(int damage)
{
	health_ -= damage;
	if (health_ <= 0) {
		health_ = 0;
		isDraw_ = false;
	}	
}
int32_t Enemy::GetBaseAttackPower() const {
	return baseAttackPower_;
}

void Enemy::SetAttackPower(int32_t power) {
	attackPower_ = power;
}


void Enemy::SetMovementSpeed(float speed) {
	movementSpeed_ = speed;
}

void Enemy::SetAttackFrequency(float frequency) {
	attackFrequency_ = frequency;
}
void Enemy::SetHealth(int32_t health) {
	health_ = health;
}
void Enemy::SetAILevel(int32_t level) {
	aiLevel_ = level;
}

void Enemy::Fire() {
	assert(player_);
	// 弾の速度
	const float kBulletSpeed = 0.5f;
	// 自キャラのワールド座標を取得
	Vector3 playerPos = player_->GetWroldPosition();
	Vector3 enemyPos = GetWorldPosition();
	Vector3 difference = playerPos - enemyPos;
	difference = normalize(difference); 
	difference *= kBulletSpeed;
	Vector3 velocity = difference;

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);
	// 弾を登録する
	bullets_.push_back(newBullet);
	
}
void Enemy::OnCollision() {
	hitEnemy += 1.0f;
	TakeDamage(player_->GetAttackPower()); 
}
void Enemy::Update() {
	
	// デスフラグの立った弾を削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// メンバ関数ポインタの呼び出し
	(this->*phaseEnemy[static_cast<size_t>(phase_)])();

	// 弾の更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}
	//-------- 敵の復活 -------//
	if (!isDraw_) {
		isAlive_++;
	}
	if (isAlive_ == 60) {
		isDraw_ = true;
		health_ = 100;
		isAlive_ = 0;
	}
	// アフィン変換行列の計算
	Matrix4x4 moveMatrix = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// ワールド行列に代入
	worldTransform_.matWorld_ = moveMatrix;
	// 行列を定数バッファに転送
	worldTransform_.Update();

	
}

void Enemy::Draw(ViewProjection& viewProjection) {
	// 3Dモデル
	if (isDraw_) {
		model_->Draw(worldTransform_, viewProjection, textureHandle_);
	}
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

