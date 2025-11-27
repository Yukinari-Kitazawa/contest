#include "CameraTitle.h"
#include "Defines.h"
using namespace DirectX;

CameraTitle::CameraTitle()
{
}

CameraTitle::~CameraTitle()
{
}

void CameraTitle::Update()
{
	// カメラ位置設定
	m_pos = { 0.0f, 0.0f, -5.0f };
	// 注視点設定
	m_look = { 0.0f, 0.0f, 0.0f };
	// 上方ベクトル設定
	m_up = { 0.0f, 1.0f, 0.0f };
	// 画角設定
	m_fovy = DirectX::XMConvertToRadians(60.0f);
	// アスペクト比設定
	m_aspect = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
	// ニアクリップ設定
	m_near = 0.1f;
	// ファークリップ設定
	m_far = 100.0f;
}