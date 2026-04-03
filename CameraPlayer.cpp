#include "CameraPlayer.h"
using namespace DirectX;
CameraPlayer::CameraPlayer() : m_radXZ(0.0f), m_radY(15.0f), m_radius(4.0f)
{
}

CameraPlayer::~CameraPlayer()
{
}

void CameraPlayer::Update()
{

	//  プレイヤー位置
	m_look = m_pPlayer->GetPos();
	XMFLOAT3 playerPos = m_pPlayer->GetPos();
	XMVECTOR vPlayerPos = XMLoadFloat3(&playerPos);

	float yaw = XMConvertToRadians(m_pPlayer->GetRotation().z);
	float pitch = XMConvertToRadians(m_pPlayer->GetRotation().x);

	
	// クォータニオン生成
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(pitch, yaw, 0.0f);
	// 回転行列に変換
	XMMATRIX rot = XMMatrixRotationQuaternion(quat);

	//カメラの位置(プレイヤーの後ろ)をoffsetBaseに格納
	XMVECTOR offsetBase = XMVectorSet(0.0f, 1.0f, -m_radius, 0.0f);//x,y,z

	//offsetBase を回転した行列をoffsetrotatedに格納
	XMVECTOR offsetRotated = XMVector3TransformCoord(offsetBase, rot);

	// 最終的なカメラ座標 = プレイヤー座標 + 回転後オフセット
	XMVECTOR vCamPos = XMVectorAdd(vPlayerPos, offsetRotated);
	XMStoreFloat3(&m_pos, vCamPos);




	if (m_radius < 0.1f) m_radius = 0.1f;//0はエラー
}
DirectX::XMFLOAT4X4 CameraPlayer::GetViewMatrix(bool transpose)
{
	DirectX::XMFLOAT4X4 mat;

	//プレイヤーの角度（クォータニオン回転用）
	float pitch = XMConvertToRadians(m_pPlayer->GetRotation().x);
	float yaw = XMConvertToRadians(m_pPlayer->GetRotation().z);
	float roll = XMConvertToRadians(m_pPlayer->GetRotation().y);

	//クォータニオン作成
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
	XMMATRIX rot = XMMatrixRotationQuaternion(quat);

	//カメラ位置・注視点（＝プレイヤー）
	XMVECTOR vEye = XMLoadFloat3(&m_pos);
	XMVECTOR vTarget = XMLoadFloat3(&m_look);

	//プレイヤーの向き（forward）と up を回転行列から直接取り出す
	XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(vTarget, vEye));
	XMVECTOR up = rot.r[1]; // Y軸
	
	XMMATRIX view = XMMatrixLookToLH(vEye, forward, up);

	if (transpose) {
		view = XMMatrixTranspose(view);
	}
	XMStoreFloat4x4(&mat, view);
	return mat;
}