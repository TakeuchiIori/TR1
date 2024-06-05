#include "WorldTransform.h"
#include "MathFunc.h"
void WorldTransform::Update() {
	// スケール・回転・平行移動行列を合成して行列を計算する
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
	// 定数バッファに転送する
	TransferMatrix();
}
