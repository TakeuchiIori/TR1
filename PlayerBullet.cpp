#include "PlayerBullet.h"

PlayerBullet::PlayerBullet() {}

PlayerBullet::~PlayerBullet() {}

void PlayerBullet::Initialize(Model* model, const Vector3& position,const Vector3& velocity) {
	// NILLポインタチェック
	assert(model);
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("./Resources./cube./cube.jpg");
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取ったワールドトランスフォームをメンバ変数に代入
	worldTransform_.translation_ = position;
	// 引数で受け取っ速度をメンバ変数に代入
	velocity_ = velocity;
}

void PlayerBullet::Update() {

	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
	// 座標を移動させる
	worldTransform_.translation_ += velocity_;
	// ワールドトランスフォームの更新
	worldTransform_.Update();
}

void PlayerBullet::OnCollision() {
	isDead_ = true;
}

Vector3 PlayerBullet::GetWroldPosition() { 
	// ワールド座標を入れる変数
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {

	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
