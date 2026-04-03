#include "SceneResult.h"
#include"Score.h"
#include"Input.h"
#include"CameraResult.h"
#include"Defines.h"
#include"Sprite.h"
SceneResult::SceneResult():m_pCamera(nullptr),m_pTexture(nullptr)
{
	m_pTexture = new Texture();
	m_pTexture->Create("Assets/texture/Result.png");
	m_pCamera = new CameraResult();
	m_pBgm = LoadSound("Assets/sound/ResultBgm.mp3",true);
	m_pspeaker = PlaySound(m_pBgm);
	m_pspeaker->SetVolume(0.5f);
	m_pSe = LoadSound("Assets/sound/Decision.mp3", false);

}

SceneResult::~SceneResult()
{
	delete m_pCamera;
	m_pCamera = nullptr;
	SAFE_DELETE(m_pTexture);
	m_pspeaker->Stop(0);
	m_pspeaker=nullptr;
	m_pSEspeaker=nullptr;
}

void SceneResult::Update()
{
	if (IsKeyTrigger(VK_SPACE))
	{
		m_pSEspeaker = PlaySound(m_pSe);
		m_pSEspeaker->SetVolume(1.0f);
		SetNext(0);
	}
	Score::GetInstanse()->Update();
}

void SceneResult::Draw()
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMMATRIX T =
		DirectX::XMMatrixTranslation(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	DirectX::XMMATRIX S;
	S = DirectX::XMMatrixScaling(1.0f, -1.0f, 1.0f);
	DirectX::XMMATRIX mWorld = S * T;
	DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(mWorld));

	Sprite::SetWorld(world);       // スプライトのワールド行列を設定 
	Sprite::SetUVPos({ 0.0f,0.0f });
	Sprite::SetUVScale({ 1.0f,1.0f });
	// フェードの表示設定 
	Sprite::SetVP();
	Sprite::SetTexture(m_pTexture);
	Sprite::SetSize({ SCREEN_WIDTH,SCREEN_HEIGHT });
	Sprite::SetOffset({ 0.0f, 0.0f });
	Sprite::SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	Sprite::Draw();
	Score::GetInstanse()->Draw();
}
