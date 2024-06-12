#include "GameScene.h"
#include "AxisIndicator.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "Vector2.h"
#include <cassert>
#include <thread>
#include <chrono>
//#include <dlib/matrix.h>
//#include <dlib/matrix/matrix_assign.h>
#include <vector>
GameScene::GameScene() : player_(nullptr), enemy_(nullptr), enemySpawnRate_(1.0f), powerUpSpawnRate_(1.0f),level(0) {}

GameScene::~GameScene() {
	delete model_;
	delete modelSkydome_;
	delete player_;
	delete enemy_;
	delete skydome_;
	delete debugCamera_;
	
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureHandle_ = TextureManager::Load("./Resources./uvChecker.png");

	//=============== 3Dモデル ===============//

	model_ = Model::Create();
	// 天球の3Dモデル
	modelSkydome_ = Model::CreateFromOBJ("night", true);

	//=============== 天球の初期化 ===============//

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_,&viewProjection_);

	// =============== 変更無し ===============//

	//  ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.farZ = 150.0f;
	viewProjection_.Initialize();
	// デバッグカメラ
	debugCamera_ = new DebugCamera(1280,720);
	isDebugCameraActive_ = false;
	// 軸方向表示の表示を有効
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
	//=============== 自キャラの生成 ===============//

	player_ = new Player();
	player_->Initialize(model_,textureHandle_);

	//=============== 敵キャラの生成 ===============//
	enemy_ = new Enemy();
	enemy_->Initialize(model_);
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);
	player_->SetPlayer(enemy_);

	

}


void GameScene::Update() { 
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
	// カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetView();
		viewProjection_.matProjection = debugCamera_->GetProjection();
		//  ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}
	player_->Update();
	if (enemy_) {
		enemy_->Update();
	}
	CheackAllCollisions();
	skydome_->Update();
	// 敵の難易度を調整する関数
	AdjustEnemyAI();
	
	
	


	// ImGuiの描画開始
	ImGui::Begin("Window");
	// プレイヤーの情報を表示
	int32_t playerHealth = player_->GetHealth(); // プレイヤーのHPを取得
	int32_t enemyHealth = enemy_->GetHealth(); // 敵のHPを取得
	// プレイヤー情報の表示
	if (ImGui::TreeNode("Player")) {
		ImGui::SliderInt("Health", &playerHealth, 0, 100);
		ImGui::Text("ShootInterval: %f", -player_->GettimeUntilNextShoot() );
		player_->SetHealth(playerHealth);
		ImGui::Text("Attack Power: %d", player_->GetAttackPower());
		ImGui::Text("Speed: %f", player_->GetSpeed());
		ImGui::Text("pressureLine: %f", pressureLine);
		ImGui::Text("recoveryLine: %f", recoveryLine);
		// 他のプレイヤー関連の情報をここに追加することもできます
		ImGui::TreePop();
	}
	// 敵情報の表示
	if (ImGui::TreeNode("Enemy")) {
		if (enemy_) {
			ImGui::Text("AI Level : %d", level);
			ImGui::SliderInt("Enemy Health", &enemyHealth, 0, 100);
			enemy_->SetHealth(enemyHealth);
			ImGui::Text("Enemy Attack Power: %d", enemy_->GetAttackPower());
			ImGui::Text("Enemy Speed: %f", enemy_->GetkLeaveSpeed().x);
			ImGui::Text("Hit Enemy: %f", enemy_->GetHitEnemy());
			ImGui::Text("Death Timer: %d", enemy_->GetEnemyDeathTimer());
			ImGui::Text("Result: %f", recoveryLine);
		}
		ImGui::TreePop();
	}
	// ImGuiの描画終了
	ImGui::End();
	ImGui::Render();
	
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	//  スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	player_->Draw(viewProjection_);
	if (enemy_) {
		enemy_->Draw(viewProjection_);
	}
	//=============== 天球の描画 ===============//
	skydome_->Draw();
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheackAllCollisions() {
	// 判定対象とAとBの座標
	Vector3 posA, posB;
	// 自弾リストの取得
	const list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();
	//==================== 自キャラと敵弾の当たり判定 ====================//
#pragma region
	// 自キャラの座標
	posA = player_->GetWroldPosition();
	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
		// 敵弾の座標d
		posB = bullet->GetWaroldPosition();
		// 座標の距離を求める
		// 座標の距離を求める
		float distance = float(std::sqrt(std::pow(posA.x - posB.x, 2) + std::pow(posA.y - posB.y, 2) + std::pow(posA.z - posB.z, 2)));
		float radDistance = player_->Setradius() + bullet->Setradius();
		// 球と球の交差判定
		if (distance <= radDistance) {
			// 自キャラの衝突時コールバック関数を呼び出す
			player_->OnCollision();
			bullet->OnCollision();
		}
	}
#pragma endregion
	//==================== 自弾と敵弾キャラの当たり判定 ====================//
#pragma region
	// 敵キャラの座標
	posA = enemy_->GetWorldPosition();
	// 敵キャラと自弾全ての当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		// 自弾の座標
		posB = bullet->GetWroldPosition();
		// 座標の距離を求める
		float distance = float(std::sqrt(std::pow(posA.x - posB.x, 2) + std::pow(posA.y - posB.y, 2) + std::pow(posA.z - posB.z, 2)));
		float radDistance = enemy_->Setradius() + bullet->Setradius();
		// 球と球の交差判定
		if (distance <= radDistance) {
			// 敵キャラの衝突時コールバック関数を呼び出す
			enemy_->OnCollision();
			bullet->OnCollision();
		}
	}

#pragma endregion
	//==================== 自弾と敵弾の当たり判定 ====================//
#pragma region
	// 敵キャラと自弾全ての当たり判定
	for (PlayerBullet* playerbullet : playerBullets) {
		for (EnemyBullet* enemybullet : enemyBullets) {
			// 自弾の座標
			posA = enemybullet->GetWaroldPosition();
			// 自弾の座標
			posB = playerbullet->GetWroldPosition();
			// 座標の距離を求める
			float distance = float(std::sqrt(std::pow(posA.x - posB.x, 2) + std::pow(posA.y - posB.y, 2) + std::pow(posA.z - posB.z, 2)));
			float radDistance = playerbullet->Setradius() + enemybullet->Setradius();
			// 球と球の交差判定
			if (distance <= radDistance) {
				// 敵キャラの衝突時コールバック関数を呼び出す
				playerbullet->OnCollision();
				enemybullet->OnCollision();
			}
		}
	}
#pragma endregion
}

void GameScene::AdjustEnemyDifficulty() {
	//if (player_ == nullptr || enemy_ == nullptr) return;

	//int32_t playerHealth = player_->GetHealth(); // プレイヤーのHPを取得

	//int32_t pressureLineHP = 70;
	//int32_t recovery = 50;
	////int32_t enemyHealth = enemy_->GetHealth(); // 敵のHPを取得

	//if (playerHealth > pressureLineHP) {
	//	// プレイヤーのHPが75以上の場合、敵の強さを増加
	//	
	//}
	//else if (playerHealth > recovery) {
	//	
	//}
}

void GameScene::AdjustEnemyAI() {
	if (!player_ || !enemy_) return;
	// 重みの定義
	std::vector<float> weights = { 0.4f, 0.3f, 0.2f, 0.4f, 0.3f, 0.2f };
	// パラメーターを重み付けして合計
	pressureLine = weightedSum(player_, enemy_, weights);
	recoveryLine = weightedSum(player_, enemy_, weights) / 2;
	//int32_t playerHealth = player_->GetHealth();
	if (player_->GetHealth() > pressureLine) {
		level = 3;
		enemy_->SetAttackPower(static_cast<int>(enemy_->GetBaseAttackPower() * 1.5));
		enemy_->SetAILevel(level);
	}
	if (player_->GetHealth() < pressureLine && player_->GetHealth() > recoveryLine) {
		level = 2;
		enemy_->SetAttackPower(enemy_->GetBaseAttackPower());
		enemy_->SetAILevel(level);
	}
	if (player_->GetHealth() < recoveryLine) {
		level = 1;
		enemy_->SetAttackPower(enemy_->GetBaseAttackPower());
		enemy_->SetAILevel(level);
	}
}
// プレイヤーと敵の距離から敵の速度を計算する関数
Vector3 GameScene:: calculateEnemySpeed(const Vector3& playerPos, const Vector3& enemyPos) {
	// 敵の最大速度と最小速度を設定
	float maxSpeed = 0.5f;
	float minSpeed = 0.1f;

	// プレイヤーと敵の距離を計算
	float dx = playerPos.x - enemyPos.x;
	float dy = playerPos.y - enemyPos.y;
	float dz = playerPos.z - enemyPos.z;
	float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

	// 距離に応じて敵の速度を計算
	float speed = maxSpeed - (maxSpeed - minSpeed) * (distance / 100.0f); // 100は適当な基準距離

	// 速度ベクトルを正規化して方向を保ちつつ速度を設定
	float factor = speed / distance;
	Vector3 velocity = { dx * factor, dy * factor, dz * factor };
	return velocity;
}
// パラメーターを重み付けして合計する関数
float GameScene::weightedSum(Player* player, Enemy* enemy, const std::vector<float>& weights) {
	float sum = 0.0f;
	sum += player->GetHealth() * weights[0];
	sum += player->GetAttackPower() * weights[1];
	sum += player->GetSpeed() * weights[2];
	sum += enemy->GetHealth() * weights[3];
	sum += enemy->GetAttackPower() * weights[4];
	sum += ((enemy->GetkLeaveSpeed().x  + enemy->GetkLeaveSpeed().y + enemy->GetkLeaveSpeed().z) * weights[5]) / 3 ;
	return sum;

}
// 平均
float GameScene::weightedSumAndAverage(Player* player, Enemy* enemy, const std::vector<float>& weights, int& numParams) {
	float sum = 0.0f;
	sum += player->GetHealth() * weights[0];
	sum += player->GetAttackPower() * weights[1];
	sum += player->GetSpeed() * weights[2];
	sum += enemy->GetHealth() * weights[3];
	sum += enemy->GetAttackPower() * weights[4];
	sum += ((enemy->GetkLeaveSpeed().x + enemy->GetkLeaveSpeed().y + enemy->GetkLeaveSpeed().z) * weights[5]) / 3;

	// パラメーターの数を更新
	numParams = 6;

	// 合計をパラメーターの数で割って平均値を計算
	float average = sum / numParams;

	return average;
}