#pragma once
#include<memory>
#include<DirectXMath.h>
#include"Texture.h"
#include"CameraTitle.h"
class Particle
{
public:
	Particle(DirectX::XMFLOAT3 InitPos) ;
	void Update();
	void Draw();
	bool IsAlive() { return m_isAlive; }
	~Particle() {};
	void SetCamera(CameraTitle* camera) { m_pCamera = camera; }
private:
	CameraTitle* m_pCamera;
	std::shared_ptr<Texture> m_pTexture;
	DirectX::XMFLOAT3 m_pos[10];
	DirectX::XMFLOAT3 m_vel[10];
	float m_life = 1.0f;
	bool m_isAlive = true;
};

