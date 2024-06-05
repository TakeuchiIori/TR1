#include "Skydome.h"

void Skydome::Initialize(Model* model, ViewProjection* viewProjection) {
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// NULLポインタチェック
	assert(model);
	// 引数の内容をメンバ変数に記録
	model_ = model;
	viewProjection_ = viewProjection;
}

void Skydome::Update() {
	//worldTransform_.scale_ = {12.0f, 12.0f, 12.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.Update();

}

void Skydome::Draw() {
model_->Draw(worldTransform_, *viewProjection_);
}
