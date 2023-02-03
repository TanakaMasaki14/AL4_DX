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
	delete objRay;

	delete modelSphere;
	delete modelGround;
	delete modelRay;

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
	Sprite::LoadTexture(1, L"Resources/Fine_Basin.jpg");
	//テクスチャ2番に読み込み
	Sprite::LoadTexture(2, L"Resources/texture.png");

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });

	// 3Dオブジェクト生成
	modelSphere = Model::LoadFromOBJ("sphere");
	modelGround = Model::LoadFromOBJ("ground");
	modelRay = Model::LoadFromOBJ("ray");

	objSphere = Object3d::Create();
	objGround = Object3d::Create();
	objRay = Object3d::Create();

	objSphere->SetModel(modelSphere);
	objGround->SetModel(modelGround);
	objRay->SetModel(modelRay);

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

	//三角形の初期値を設定
	triangle.p0 = XMVectorSet(-1.0f, 0.0f, -1.0f, 1.0f);	//左手前
	triangle.p1 = XMVectorSet(-1.0f, 0.0f, +1.0f, 1.0f);	//左奥
	triangle.p2 = XMVectorSet(+1.0f, 0.0f, -1.0f, 1.0f);	//右手前
	triangle.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);	//上向き

	//レイの初期値を設定
	ray.start = XMVectorSet(0.0f, 5.0f, 0.0f, 1.0f);		//原点やや上
	ray.dir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);			//下向き

	scene = SxP;
}

void GameScene::Update()
{
	XMVECTOR sphereMoveY;
	XMVECTOR interSP;
	bool hitSP;
	XMFLOAT4 colorSP = objSphere->GetColor();

	std::ostringstream spherestr;
	std::ostringstream raystr;

	XMVECTOR rayMoveX;
	XMVECTOR interRP;
	float distanceRP;
	bool hitRP;
	XMFLOAT4 colorRP = objGround->GetColor();

	XMVECTOR interRS;
	float distanceRS;
	bool hitRS;
	XMFLOAT4 colorRS = objGround->GetColor();

	switch (scene)
	{
	case SxP:
		//球移動
		sphereMoveY = XMVectorSet(0.0f, 0.03f, 0.0f, 0.0f);
		// 移動の計算
		if (input->PushKey(DIK_UP)) {
			sphere.center += sphereMoveY;
		}
		if (input->PushKey(DIK_DOWN)) {
			sphere.center -= sphereMoveY;
		}

		objSphere->SetPosition({ sphere.center.m128_f32[0]
		,sphere.center.m128_f32[1],sphere.center.m128_f32[2] });

		spherestr << "Sphere:("
			<< std::fixed << std::setprecision(2)		//小数点以下2桁まで
			<< sphere.center.m128_f32[0] << ","			//x
			<< sphere.center.m128_f32[1] << ","			//y
			<< sphere.center.m128_f32[2] << ")";		//z

		//debugText.Print(spherestr.str(), 50, 180, 1.0f);

		//球と平面の当たり判定
		hitSP = Collision::ChackSphere2Plane(sphere, plane, &interSP);
		if (hitSP)
		{
			colorSP = { 1.0f,0.0f,0.0f,1.0f };
			objSphere->SetColor(colorSP);

			spherestr.str("");
			spherestr.clear();
			spherestr << "("
				<< std::fixed << std::setprecision(2)
				<< interSP.m128_f32[0] << ","
				<< interSP.m128_f32[1] << ","
				<< interSP.m128_f32[2] << ")";
		}
		else
		{
			colorSP = { 1.0f,1.0f,1.0f,1.0f };
			objSphere->SetColor(colorSP);
		}

		if (input->TriggerKey(DIK_SPACE))
		{
			ray.start = XMVectorSet(0.0f, 5.0f, 0.0f, 1.0f);
			colorSP = { 1.0f,1.0f,1.0f,1.0f };
			objSphere->SetColor(colorSP);
			scene = RxP;
			break;
		}

		break;

	case RxP:
		//レイ移動

		rayMoveX = XMVectorSet(0.05f, 0.00f, 0.0f, 0.0f);
		// 移動の計算

		if (input->PushKey(DIK_RIGHT)) {
			ray.start += rayMoveX;
		}
		if (input->PushKey(DIK_LEFT)) {
			ray.start -= rayMoveX;
		}

		objRay->SetPosition({ ray.start.m128_f32[0]
		,ray.start.m128_f32[1],ray.start.m128_f32[2] });

		raystr << "Ray Start:("
			<< std::fixed << std::setprecision(2)
			<< ray.start.m128_f32[0] << ","		
			<< ray.start.m128_f32[1] << ","		
			<< ray.start.m128_f32[2] << ")";	

		hitRP = Collision::ChackRay2Plane(ray, plane, &distanceRP, &interRP);
		if (hitRP)
		{
			colorRP = { 1.0f,0.0f,0.0f,1.0f };
			objGround->SetColor(colorRP);
			raystr.str("");
			raystr.clear();
			raystr << "("
				<< std::fixed << std::setprecision(2)
				<< interRP.m128_f32[0] << ","
				<< interRP.m128_f32[1] << ","
				<< interRP.m128_f32[2] << ")";
		}
		else
		{
			colorRP = { 1.0f,1.0f,1.0f,1.0f };
			objGround->SetColor(colorRP);
		}
		if (input->TriggerKey(DIK_SPACE))
		{
			ray.start = XMVectorSet(0.0f, 5.0f, 0.0f, 1.0f);
			colorRP = { 1.0f,1.0f,1.0f,1.0f };
			objGround->SetColor(colorRP);
			scene = RxS;
			break;
		}
		break;

	case RxS:
		//レイ移動

		rayMoveX = XMVectorSet(0.05f, 0.00f, 0.0f, 0.0f);
		// 移動の計算
		if (input->PushKey(DIK_RIGHT)) {
			ray.start += rayMoveX;
		}
		if (input->PushKey(DIK_LEFT)) {
			ray.start -= rayMoveX;
		}

		objRay->SetPosition({ ray.start.m128_f32[0]
		,ray.start.m128_f32[1],ray.start.m128_f32[2] });

		raystr << "Ray Start:("
			<< std::fixed << std::setprecision(2)
			<< ray.start.m128_f32[0] << ","		
			<< ray.start.m128_f32[1] << ","		
			<< ray.start.m128_f32[2] << ")";	

		hitRS = Collision::ChackRay2Sphere(ray, sphere, &distanceRS, &interRS);
		if (hitRS)
		{
			colorRS = { 1.0f,1.0f,1.0f,1.0f };
			objRay->SetColor(colorRS);
			colorRS = { 1.0f,0.0f,0.0f,1.0f };
			objSphere->SetColor(colorRS);
			//debugText.Print("HIT!", 700, 240, 1.0f);

			raystr.str("");
			raystr.clear();
			raystr << "("
				<< std::fixed << std::setprecision(2)
				<< interRS.m128_f32[0] << ","
				<< interRS.m128_f32[1] << ","
				<< interRS.m128_f32[2] << ")";
		}
		else
		{
			colorRS = { 1.0f,1.0f,1.0f,1.0f };
			objRay->SetColor(colorRS);
			colorRS = { 1.0f,1.0f,1.0f,1.0f };
			objSphere->SetColor(colorRS);
		}
		if (input->TriggerKey(DIK_SPACE))
		{
			sphere.center = XMVectorSet(0.0f, 2.0f, 0.0f, 1.0f);	
			colorRS = { 1.0f,1.0f,1.0f,1.0f };
			objGround->SetColor(colorRS);
			scene = SxP;
			break;
		}
		break;

	}
	// カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		if (input->PushKey(DIK_S)) { Object3d::CameraMoveVector({ 0.0f,-0.3f,0.0f }); }
		else if (input->PushKey(DIK_W)) { Object3d::CameraMoveVector({ 0.0f,+0.3f,0.0f }); }
		if (input->PushKey(DIK_A)) { Object3d::CameraMoveVector({ -0.3f,0.0f,0.0f }); }
		else if (input->PushKey(DIK_D)) { Object3d::CameraMoveVector({ +0.3f,0.0f,0.0f }); }
	}

	objSphere->Update();
	objGround->Update();
	objRay->Update();
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

	// 3Dオブジェクトの描画
	switch (scene)
	{
	case SxP:
		objSphere->Draw();
		objGround->Draw();
		break;

	case RxP:
		objGround->Draw();
		objRay->Draw();
		break;

	case RxS:
		objSphere->Draw();
		objRay->Draw();

		break;

	}

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
