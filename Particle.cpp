#include "Particle.h"
#include<random>
#include"Defines.h"
#include"Sprite.h"

Particle::Particle(DirectX::XMFLOAT3 InitPos)
{
	for (auto& pos : m_pos)
	{
		pos = InitPos;
	}
	
	m_pTexture = std::make_shared<Texture>();
	m_pTexture->Create("Assets/texture/particle.png");


	for (auto& vel : m_vel)
	{
		//　ランダムな方向（XY）を設定
		vel = { (float)(rand() % 200 - 100) / 100.0f, (float)(rand() % 200 - 100) / 100.0f ,0.0f };
	}
}

void Particle::Update()
{
	for (int i = 0;i < 10;i++)
	{
		// 速度を加算
		m_pos[i].x += m_vel[i].x*0.01f;
		m_pos[i].y += m_vel[i].y*0.01f;
	}
	m_life -= 0.01f;
	if (m_life <= 0.0f) {
		m_isAlive = false;
	}

}
void Particle::Draw()
{






	DirectX::XMFLOAT4X4 world;
	DirectX::XMMATRIX T;
	DirectX::XMMATRIX S;
	DirectX::XMFLOAT4X4 wvp[3];
	DirectX::XMMATRIX mWorld;

	SetBlendMode(BlendMode::BLEND_ALPHA);
	Sprite::SetSize({ 3.0f,3.0f });
	Sprite::SetUVPos({ 0.0f,0.0f });
	Sprite::SetUVScale({ 1.0f,1.0f });
	wvp[1] = m_pCamera->GetViewMatrix();
	wvp[2] = m_pCamera->GetProjectionMatrix();
	for (int i = 0;i < 10;i++) {
		T =
			DirectX::XMMatrixTranslation(m_pos[i].x, m_pos[i].y, m_pos[i].z);
		S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
		mWorld = S * T;
		DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(mWorld));
	//ビルボードに
	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&wvp[1]));
	invView.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
	DirectX::XMMATRIX mat = S * invView * T;
	DirectX::XMStoreFloat4x4(&wvp[0], DirectX::XMMatrixTranspose(mat));
		Sprite::SetWorld(wvp[0]);     // スプライトのワールド行列を設定 
	Sprite::SetView(m_pCamera->GetViewMatrix());
	Sprite::SetProjection(m_pCamera->GetProjectionMatrix());
		Sprite::SetTexture(m_pTexture.get());
	
		
	


		//Sprite::SetVP();
		Sprite::SetColor({ 1.0f, 1.0f, 1.0f, m_life });
		Sprite::Draw();
	}
}