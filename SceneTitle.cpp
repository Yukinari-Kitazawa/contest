#include "SceneTitle.h"
#include"Input.h"
#include"Defines.h"
#include"CameraTitle.h"
#include"Score.h"
#include"Sound.h"
#include"WorldtoScreen.h"
#include<time.h>

SceneTitle::SceneTitle() : m_pLogo(nullptr), m_pTitle(nullptr), m_pEnter(nullptr),
m_pTitleCamera(nullptr), m_pSkyDome(nullptr), m_pBgm(nullptr), m_pSe(nullptr),
m_pspeaker(nullptr), m_pspeaker2(nullptr)
{
	m_pLogo = new Texture();
	m_pTitle = new Texture();
	m_pEnter = new Texture();
	m_pTitleCamera = new CameraTitle();
	m_pSkyDome = new SkyDome();
	m_pSkyDome->SetCamera(m_pTitleCamera);
	srand((unsigned int)time(NULL));
	m_pTitlePlayer = new TitlePlayer({ (float)(rand() % 20 - 1), (float)(rand() % 20 + 5),10.0f });
	m_pTitlePlayer->SetCamera(m_pTitleCamera);
	m_pTitlePlayer->SetRotateY(180.0f);
	for(int i=0;i<5;i++){
		DirectX::XMFLOAT3 pos = { (float)(rand() % 20 - 1), (float)(rand() % 20 +5),10.0f };
		TitleTrashObject* pTitleTrashObject = new TitleTrashObject(pos);
		m_TitleTrashObjectList.push_back(pTitleTrashObject);
	}
	//カメラのセット
	for(auto& obj : m_TitleTrashObjectList) {
		obj->SetCamera(m_pTitleCamera);
	}


	// サウンド読み込み
	m_pBgm = LoadSound("Assets/sound/TitleBgm.mp3",true);
	m_pSe = LoadSound("Assets/sound/Decision.mp3",false);
	m_pCollisionSe = LoadSound("Assets/sound/Get.mp3", false);
	m_pspeaker = PlaySound(m_pBgm);
	m_pspeaker->SetVolume(0.5f);
	// スコア初期化
	Score::Reset();
	// テクスチャ読み込み
if (FAILED(m_pTitle->Create("Assets/texture/Title.png")))
	MessageBox(NULL, "Load failed SceneTitle.", "Error", MB_OK);
if (FAILED(m_pLogo->Create("Assets/texture/TitleLogo.png")))
		MessageBox(NULL, "Load failed SceneTitle.", "Error", MB_OK);
if (FAILED(m_pEnter->Create("Assets/texture/PressEnter.png")))
		MessageBox(NULL, "Load failed SceneTitle.", "Error", MB_OK);
}

SceneTitle::~SceneTitle()
{
	if (m_pLogo) {
		delete m_pLogo;
		m_pLogo = nullptr;
	}	
	if (m_pTitle) {
		delete m_pTitle;
		m_pTitle = nullptr;
	}	
	if (m_pEnter) {
		delete m_pEnter;
		m_pEnter = nullptr;
	}
	if (m_pTitleCamera) {
		delete m_pTitleCamera;
		m_pTitleCamera = nullptr;
	}
	if (m_pSkyDome) {
		delete m_pSkyDome;
		m_pSkyDome = nullptr;
	}
	if (m_pTitlePlayer)
	{
		delete m_pTitlePlayer;
		m_pTitlePlayer = nullptr;
	}
	m_pspeaker->Stop(0);
	m_pBgm = nullptr;
	m_pSe = nullptr;
	m_pCollisionSe = nullptr;
	for(auto& obj : m_TitleTrashObjectList) {
		delete obj;
	}
	m_TitleTrashObjectList.clear();
}

void SceneTitle::Update()
{
	if (IsKeyTrigger(VK_RETURN)) {
		m_pspeaker2 = PlaySound(m_pSe);
		m_pspeaker2->SetVolume(1.0f);
		SetNext(1); // 切り替え先のシーンを設定（１はゲーム 
	}
	m_pTitleCamera->Update();
	m_pSkyDome->Update();
	for (auto& obj : m_TitleTrashObjectList) {
		obj->Update();
	}
	// 衝突判定と解決
	for(int i=0;i<m_TitleTrashObjectList.size();i++){
		for(int j=i+1;j<m_TitleTrashObjectList.size();j++){
			if (m_TitleTrashObjectList[i]->ResolveCollision(
				*m_TitleTrashObjectList[i],
				*m_TitleTrashObjectList[j]))
			{
				DirectX::XMFLOAT3 worldPos = m_TitleTrashObjectList[i]->GetPos();

				//// ワールド座標 → 画面座標（原点を画面中央とみなしてずらす）

				//DirectX::XMFLOAT3 Scale = { 1.0f,1.0f,1.0f };
				//DirectX::XMMATRIX ScaleMatrix =
				//	DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
				//DirectX::XMMATRIX TransMatrix =
				//	DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
				//DirectX::XMMATRIX WorldMatrix = ScaleMatrix * TransMatrix;

				//DirectX::XMFLOAT4X4 world ,view, proj;

				//XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(WorldMatrix));

				//DirectX::XMMATRIX mView =
				//	DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(
				//		DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),//カメラのポジション
				//		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),	//フォーカスポジション
				//		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
				//DirectX::XMStoreFloat4x4(&view, mView);
				//DirectX::XMMATRIX mProj =
				//	DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(
				//		0.0f, (float)SCREEN_WIDTH,
				//		(float)SCREEN_HEIGHT, 0.0f,
				//		CMETER(30.0f),
				//		METER(200.0f)));
				//DirectX::XMStoreFloat4x4(&proj, mProj);

				//DirectX::XMFLOAT3 screenPos = WorldToScreen(worldPos, view, proj);
				//
				//screenPos.z = 0.0f;

				
				auto p = std::make_shared<Particle>(worldPos);
				p->SetCamera(static_cast<CameraTitle*>(m_pTitleCamera));
				m_pParticle.push_back(p);
				// 衝突音再生
				m_pspeaker3 = PlaySound(m_pCollisionSe);

			}
		}
		if (m_TitleTrashObjectList[i]->ResolveCollision(*m_TitleTrashObjectList[i], *m_pTitlePlayer))
		{
			DirectX::XMFLOAT3 worldPos = m_TitleTrashObjectList[i]->GetPos();

			//// ワールド座標 → 画面座標（原点を画面中央とみなしてずらす）

			//DirectX::XMFLOAT3 Scale = { 1.0f,1.0f,1.0f };
			//DirectX::XMMATRIX ScaleMatrix =
			//	DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
			//DirectX::XMMATRIX TransMatrix =
			//	DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
			//DirectX::XMMATRIX WorldMatrix = ScaleMatrix * TransMatrix;

			//DirectX::XMFLOAT4X4 world ,view, proj;

			//XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(WorldMatrix));

			//DirectX::XMMATRIX mView =
			//	DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(
			//		DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),//カメラのポジション
			//		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),	//フォーカスポジション
			//		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
			//DirectX::XMStoreFloat4x4(&view, mView);
			//DirectX::XMMATRIX mProj =
			//	DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(
			//		0.0f, (float)SCREEN_WIDTH,
			//		(float)SCREEN_HEIGHT, 0.0f,
			//		CMETER(30.0f),
			//		METER(200.0f)));
			//DirectX::XMStoreFloat4x4(&proj, mProj);

			//DirectX::XMFLOAT3 screenPos = WorldToScreen(worldPos, view, proj);
			//
			//screenPos.z = 0.0f;

			// ★ ここでパーティクルを1回だけ生成 ★
			auto p = std::make_shared<Particle>(worldPos);
			p->SetCamera(static_cast<CameraTitle*>(m_pTitleCamera));
			m_pParticle.push_back(p);
			// 衝突音再生
			m_pspeaker3 = PlaySound(m_pCollisionSe);
		}
	}

	//プレイヤーとの衝突

	//パーティクル削除
	for (auto it = m_pParticle.begin(); it != m_pParticle.end(); ) {
		if (!(*it)->IsAlive()) {
			it = m_pParticle.erase(it);
		} else {
			++it;
		}
	}




	m_pTitlePlayer->Update();
}

void SceneTitle::Draw()
{
	m_pSkyDome->Draw();
	for (auto& obj : m_TitleTrashObjectList) {
		obj->Draw();
	}
	m_pTitlePlayer->Draw();
	DirectX::XMFLOAT4X4 world;
	DirectX::XMMATRIX T =
		DirectX::XMMatrixTranslation(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0.0f);
	DirectX::XMMATRIX S;
		S = DirectX::XMMatrixScaling(1.0f, -1.0f, 1.0f);
	DirectX::XMMATRIX mWorld = S * T;

	//Sprite::SetWorld(world);       // スプライトのワールド行列を設定 
	//Sprite::SetUVPos({0.0f,0.0f});
	//Sprite::SetUVScale({ 1.0f,1.0f });
	//Sprite::SetVP();
	//Sprite::SetTexture(m_pTitle);
	//Sprite::SetSize({ SCREEN_WIDTH,SCREEN_HEIGHT});
	//Sprite::SetOffset({ 0.0f, 0.0f });
	//Sprite::SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	//Sprite::Draw();
	RenderTarget* pRTV = GetDefaultRTV(); // RenderTargetView 
	SetRenderTargets(1, &pRTV, nullptr);  // 3 null 2D表示になる

	for (auto& particle : m_pParticle) {
		particle->Update();
		particle->Draw();
	}

	T =
		DirectX::XMMatrixTranslation(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0.0f);

		S = DirectX::XMMatrixScaling(1.0f, -1.0f, 1.0f);
	mWorld = S * T;
	DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(mWorld));

	Sprite::SetWorld(world);       // スプライトのワールド行列を設定 
	Sprite::SetVP();
	Sprite::SetTexture(m_pLogo);
	Sprite::SetSize({1280.0f,320.0f});
	Sprite::SetOffset({ 0.0f, 0.0f });
	Sprite::SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	Sprite::Draw();
	T =
		DirectX::XMMatrixTranslation(SCREEN_WIDTH/2, SCREEN_HEIGHT/2+200.0f, 0.0f);
	
		S = DirectX::XMMatrixScaling(1.0f, -1.0f, 1.0f);
	mWorld = S * T;
	DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(mWorld));

	Sprite::SetWorld(world);       // スプライトのワールド行列を設定 
	Sprite::SetVP();
	Sprite::SetTexture(m_pEnter);
	Sprite::SetSize({1080.0f,140.0f});
	Sprite::SetOffset({ 0.0f, 0.0f });
	Sprite::SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	Sprite::Draw();
}
