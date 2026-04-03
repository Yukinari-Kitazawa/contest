#pragma once
#include "Camera.h" 
#include "Input.h" 
#include"Player.h"

class CameraPlayer:public Camera
{
public:
	CameraPlayer();
	~CameraPlayer();
	void Update()final;
	void SetPlayer(Player* Inplayer) { m_pPlayer = Inplayer; }
	DirectX::XMFLOAT4X4 GetViewMatrix(bool transpose =true)override;
private:
	Player* m_pPlayer;
	float m_radXZ;
	float m_radY;
	float m_radius; //  
};