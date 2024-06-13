#include "Player.h"
#include "Enemy.h"
#include <ctime>
#define NOMINMAX
Player::Player() {}

Player::~Player() {
	// bullet_の解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Initialize(Model* model, uint32_t textureHandle) {
	// NILLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0,-5, -20.0f };
	// シングルインスタンスを取得
	input_ = Input::GetInstance();
	if (heal) {
		health_ = 100;
	}
}

void Player::Update() {
	// 旋回処理
	//Rotate();
	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	//==================== キーボード入力による移動処理 ====================//
	// キャラクターの移動ベクトル
	Vector3 move = { 0.0f, 0.0f, 0.0f };
	// キャラクターの速さ
	
	// 押した方向で移動ベクトルを変更　（左右）
	if (input_->PushKey(DIK_A)) {
		move.x -= kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_D)) {
		move.x += kCharacterSpeed;
	}
	// 押した方向で移動ベクトルを変更　（上下）
	if (input_->PushKey(DIK_W)) {
		move.y += kCharacterSpeed;
	}
	else if (input_->PushKey(DIK_S)) {
		move.y -= kCharacterSpeed;
	}
	worldTransform_.translation_ += move;
	//----- 移動限界座標 -----//
	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;
	const float kMoveLimitZ = 30.0f;
	// 範囲を超えない処理
	worldTransform_.translation_.x = clamp(worldTransform_.translation_.x, -kMoveLimitX, kMoveLimitX);
	worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, -kMoveLimitY, kMoveLimitY);
	worldTransform_.translation_.z = clamp(worldTransform_.translation_.z, -kMoveLimitZ, kMoveLimitZ);
	//==================== キャラクターの攻撃処理 ====================//
	Attack();
	// 弾の更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
	//アフィン変換行列の計算
	Matrix4x4 moveMatrix = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// ワールド行列に代入
	worldTransform_.matWorld_ = moveMatrix;
	// 行列を定数バッファに転送
	worldTransform_.Update();
	Heal();
}

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデル
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}
void Player::Rotate() {
	// 回転速さ［ラジアン / frame］
	const float kRotSpeed = 0.02f;
	const float kMaxRotation = 0.4f;  // Maximum rotation limit
	const float kMinRotation = -0.4f; // Minimum rotation limit
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed; // Rotate left
		if (worldTransform_.rotation_.y < kMinRotation) {
			worldTransform_.rotation_.y = kMinRotation; // Clamp to minimum rotation
		}
	}else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed; // Rotate right
		if (worldTransform_.rotation_.y > kMaxRotation) {
			worldTransform_.rotation_.y = kMaxRotation; // Clamp to maximum rotation
		}
	}
}
void Player::TakeDamage(int damage)
{
	// ダメージを受けたときの処理
	health_ -= damage;
	if (health_ <= 0) {
		health_ = 0;
		// HPが0以下になったときはカウントを停止する
		isHealCounting_ = false;
	}
	else {
		// HPが100未満の場合はカウントを開始する
		isHealCounting_ = true;
		// ダメージを受けた時刻を記録
		lastDamageTime_ = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}
}
void Player::Heal() {
	const int kHealAmount = 10;
	const int kMaxHealth = 100;
	auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	auto timeSinceLastDamage = currentTime - lastDamageTime_;
	// HPが100未満であり、かつカウント中の場合に回復を開始
	if ( health_ < kMaxHealth && timeSinceLastDamage >= healCooldown_) {
		if (timeSinceLastDamage % static_cast<int>(healInterval_) == 0) {
			health_ += kHealAmount;
			if (health_ > kMaxHealth) {
				health_ = kMaxHealth;
			}
			lastDamageTime_ = currentTime; // 回復した時刻を更新
		}
	}
}
void Player::OnCollision() {
	HitPlayer += 1.0f;
	if (HitPlayer > 5.0f) {
		kCharacterSpeed *= 1.25f;
		HitPlayer = 0.0f;
	}
	TakeDamage(enemy_->GetAttackPower());
}

void Player::Attack() {
	// 現在の時間を取得
	time_t currentTime = time(nullptr);

	// 次の発射までの経過時間を計算（秒単位）
	timeUntilNextShoot = float(difftime(nextShootTime_, currentTime));

	// 次の発射までの時間が経過しており、かつスペースキーが押された場合に発射する
	if (input_->TriggerKey(DIK_SPACE) && timeUntilNextShoot <= 0) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		// 速度ベクトルを自機の向きに合わせて回転させる
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);

		// 次の発射時刻を更新
		nextShootTime_ = currentTime + static_cast<time_t>(shootInterval_);

		// 最後の発射時刻も更新
		lastShootTime_ = currentTime;
	}

	if (timeUntilNextShoot <= -7) {
		isPenaltyActive_ = true;
		timeUntilNextShoot = 0;
	}
	else {
		isPenaltyActive_ = false;
	}
}

Vector3 Player::GetWroldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}






