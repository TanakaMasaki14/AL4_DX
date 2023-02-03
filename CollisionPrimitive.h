#pragma once
#include <DirectXMath.h>

//球
struct Sphere
{
	//中心座標
	DirectX::XMVECTOR center = { 0.0f,0.0f,0.0f,1.0f };
	//半径
	float radius = 1.0f;
};
//平面
struct Plane
{
	//法線ベクトル
	DirectX::XMVECTOR normal = { 0.0f,1.0f,0.0f,0.0f };
	//原点からの座標
	float distance = 0.0f;
};

struct Triangle
{
	//頂点座標3つ
	DirectX::XMVECTOR	p0;
	DirectX::XMVECTOR	p1;
	DirectX::XMVECTOR	p2;

	//法線ベクトル
	DirectX::XMVECTOR	normal;
};