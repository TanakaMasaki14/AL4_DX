#include "GameScene.h"
#include <cassert>
#include "Collision.h"
#include <sstream>
#include <iomanip>

using namespace DirectX;

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete spriteBG;
	delete objSphere;
	delete objGround;
	delete modelSphere;
	delete modelGround;

	//前景スプライト解放
	delete sprite1;
	delete sprite2;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);

	this->dxCommon = dxCommon;
	this->input = input;

	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// デバッグテキスト用テクスチャ読み込み
	Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	Sprite::LoadTexture(1, L"Resources/background.png");
	//テクスチャ2番に読み込み
	Sprite::LoadTexture(2, L"Resources/texture.png");

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });

	// 3Dオブジェクト生成
	modelSphere = Model::LoadFromOBJ("sphere");
	modelGround = Model::LoadFromOBJ("ground");

	objSphere = Object3d::Create();
	objGround = Object3d::Create();

	objSphere->SetModel(modelSphere);
	objGround->SetModel(modelGround);

	//前景スプライト生成
	//座標{0,0}にテクスチャ2番のスプライトを生成
	sprite1 = Sprite::Create(2, { 0.0f,0.0f });
	//座標{500,500}にテクスチャ2番のスプライトを生成
	sprite2 = Sprite::Create(2, { 500.0f,500.0f }, { 1.0f,0.0f,0.0f,1.0f }, { 0,0 }, false, true);

	//球の初期値を設定
	sphere.center = XMVectorSet(0.0f, 2.0f, 0.0f, 1.0f);	//中心点座標
	sphere.radius = 1.0f;	//半径

	//平面の初期値を設定
	plane.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);		//法線ベクトル
	plane.distance = 0.0f;	//原点からの座標

	//三角形の初期値の設定
	triangle.p0 = XMVectorSet(-1.0f, 0.0f, -1.0f, 1.0f);
	triangle.p1 = XMVectorSet(-1.0f, 0.0f, +1.0f, 1.0f);
	triangle.p2 = XMVectorSet(+1.0f, 0.0f, -1.0f, 1.0f);
	triangle.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
}

void GameScene::Update()
{
	// オブジェクト移動
	//if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	//{
	//	// 現在の座標を取得
	//	XMFLOAT3 position = objSphere->GetPosition();

	//	// 移動後の座標を計算
	//	if (input->PushKey(DIK_UP)) { position.y += 1.0f; }
	//	else if (input->PushKey(DIK_DOWN)) { position.y -= 1.0f; }
	//	if (input->PushKey(DIK_RIGHT)) { position.x += 1.0f; }
	//	else if (input->PushKey(DIK_LEFT)) { position.x -= 1.0f; }

	//	// 座標の変更を反映
	//	objSphere->SetPosition(position);
	//}
	//球移動
	XMVECTOR moveY = XMVectorSet(0.0f, 0.01f, 0.0f, 0.0f);
	if (input->PushKey(DIK_UP)) { sphere.center += moveY; }
	else if (input->PushKey(DIK_DOWN)) { sphere.center -= moveY; }

	XMVECTOR moveX = XMVectorSet(0.01f, 0.0f, 0.0f, 0.0f);
	if (input->PushKey(DIK_RIGHT)) { sphere.center += moveX; }
	else if (input->PushKey(DIK_LEFT)) { sphere.center -= moveX; }

	//stringstreamで変数の値を埋め込んで整形する
	std::ostringstream spherestr;
	spherestr << "Sphere:("
		<< std::fixed << std::setprecision(2)		//小数点以下2桁まで
		<< sphere.center.m128_f32[0] << ","			//x
		<< sphere.center.m128_f32[1] << ","			//y
		<< sphere.center.m128_f32[2] << ")";		//z

	debugText.Print(spherestr.str(), 50, 180, 1.0f);

	////球と平面の当たり判定
	//XMVECTOR inter;
	//bool hit = Collision::ChackSphere2Plane(sphere, plane, &inter);
	//if (hit)
	//{
	//	debugText.Print("HIT Plane!", 50, 200, 1.0f);

	//	//stringstreamをリセットし、交点座標を埋め込む
	//	spherestr.str("");
	//	spherestr.clear();
	//	spherestr << "("
	//		<< std::fixed << std::setprecision(2)
	//		<< inter.m128_f32[0] << ","
	//		<< inter.m128_f32[1] << ","
	//		<< inter.m128_f32[2] << ")";

	//	debugText.Print(spherestr.str(), 50, 220, 1.0f);
	//}

	//球と三角形の当たり判定
	XMVECTOR inter;
	bool hit = Collision::ChackSphere2Triangle(sphere, triangle, &inter);
	if (hit)
	{
		debugText.Print("HIT Triangle!", 50, 240, 1.0f);

		//stringstreamをリセットし、交点座標を埋め込む
		spherestr.str("");
		spherestr.clear();
		spherestr << "("
			<< std::fixed << std::setprecision(2)
			<< inter.m128_f32[0] << ","
			<< inter.m128_f32[1] << ","
			<< inter.m128_f32[2] << ")";

		debugText.Print(spherestr.str(), 50, 260, 1.0f);
	}


	//スペースキースプライト移動
	if (input->PushKey(DIK_SPACE))
	{
		//現在の座標を取得
		XMFLOAT2 position = sprite1->GetPosition();
		//移動後の座標を計算
		position.x += 1.0f;
		//座標の変更を反映
		sprite1->SetPosition(position);
	}
	// カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		if (input->PushKey(DIK_W)) { Object3d::CameraMoveVector({ 0.0f,-1.0f,0.0f }); }
		else if (input->PushKey(DIK_S)) { Object3d::CameraMoveVector({ 0.0f,+1.0f,0.0f }); }
		if (input->PushKey(DIK_D)) { Object3d::CameraMoveVector({ -1.0f,0.0f,0.0f }); }
		else if (input->PushKey(DIK_A)) { Object3d::CameraMoveVector({ +1.0f,0.0f,0.0f }); }
	}

	objSphere->SetPosition({ sphere.center.m128_f32[0]
,sphere.center.m128_f32[1],sphere.center.m128_f32[2] });

	objSphere->Update();
	objGround->Update();
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	spriteBG->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(cmdList);

	// 3Dオブクジェクトの描画
	objSphere->Draw();
	objGround->Draw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	//sprite1->Draw();
	//sprite2->Draw();
	// デバッグテキストの描画
	debugText.DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}
