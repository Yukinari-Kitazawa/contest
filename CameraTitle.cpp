#include "CameraTitle.h"
#include "Defines.h"
using namespace DirectX;

#define TITLE_CAMERA_FOVY (60.0f)
#define TITLE_CAMERA_NEAR (0.1f)
#define TITLE_CAMERA_FAR (100.0f)
#define TITLE_CAMERA_POS (DirectX::XMFLOAT3(0.0f,0.0f,-5.0f))
#define TITLE_CAMERA_LOOK (DirectX::XMFLOAT3(0.0f,0.0f,0.0f))
#define TITLE_CAMERA_UP (DirectX::XMFLOAT3(0.0f,1.0f,0.0f))

CameraTitle::CameraTitle()
{
}

CameraTitle::~CameraTitle()
{
}

void CameraTitle::Update()
{
	// カメラ位置設定
	m_pos = { TITLE_CAMERA_POS };
	// 注視点設定
	m_look = { TITLE_CAMERA_LOOK };
	// 上方ベクトル設定
	m_up = { TITLE_CAMERA_UP };
	// 画角設定
	m_fovy = DirectX::XMConvertToRadians(TITLE_CAMERA_FOVY);
	// アスペクト比設定
	m_aspect = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
	// ニアクリップ設定
	m_near = TITLE_CAMERA_NEAR;
	// ファークリップ設定
	m_far = TITLE_CAMERA_FAR;
}