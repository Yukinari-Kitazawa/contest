#include "Main.h"
#include <memory>
#include "DirectX.h"
#include "Geometory.h"
#include "Sprite.h"
#include "Input.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneResult.h"
#include "Defines.h"
#include "ShaderList.h"
#include"FadeBlack.h"
#include"Score.h"
#include"ModelCache.h"
#include"Sound.h"
Scene* g_pScene; // シーン 
Fade* g_pFade; // フェード
//--- グローバル変数
SceneGame* g_pGame;


HRESULT Init(HWND hWnd, UINT width, UINT height)
{
	HRESULT hr;
	// DirectX初期化
	hr = InitDirectX(hWnd, width, height, false);
	if (FAILED(hr)) { return hr; }
	// モデル読み込み
	ModelCache::Init();
	ModelLoad();
	// シーン作成 
	// フェード作成 
	g_pFade = new FadeBlack();
	g_pFade->SetFade(1.0f, true);
	g_pScene = new SceneTitle();
	g_pScene->SetFade(g_pFade); // シーンに使用するフェードを設定
	// 他機能初期化
	Geometory::Init();
	Sprite::Init();
	InitInput();
	ShaderList::Init();
	Score::Init();
	

	return hr;
}

void Uninit()
{

	delete g_pScene;
	delete g_pFade;
//	delete g_pGame;
	ModelCache::UnInit();
	Score::Uninit();
	ShaderList::Uninit();
	UninitInput();
	Sprite::Uninit();
	Geometory::Uninit();
	UninitDirectX();
}

void Update()
{
	UpdateInput();
	g_pScene->RootUpdate();

	// シーン切り替え判定 
	if (g_pScene->ChangeScene()) {
		// 次のシーンの情報を取得 
		int scene = g_pScene->NextScene();

		// 現在のシーンを削除 
		delete g_pScene;

		// シーンの切り替え 
		switch (scene) {
		case 0: g_pScene = new SceneTitle();break; // TITLE 
		case 1: g_pScene = new SceneGame(); break; // GAME 
		case 2: g_pScene = new SceneResult(); break;//RESULT
		}

		// 次シーンに向けて初期設定 
		g_pFade->Start(true);   // フェード開始 
		g_pScene->SetFade(g_pFade); // フェードクラスをシーンに設定 
	}
	/*g_pGame->Update();*/
}

void Draw()
{
	BeginDrawDirectX();

	// 軸線の表示
#ifdef _DEBUG
	// グリッド
	DirectX::XMFLOAT4 lineColor(0.5f, 0.5f, 0.5f, 1.0f);
	float size = DEBUG_GRID_NUM * DEBUG_GRID_MARGIN;
	for (int i = 1; i <= DEBUG_GRID_NUM; ++i)
	{
		float grid = i * DEBUG_GRID_MARGIN;
		DirectX::XMFLOAT3 pos[2] = {
			DirectX::XMFLOAT3(grid, 0.0f, size),
			DirectX::XMFLOAT3(grid, 0.0f,-size),
		};
		Geometory::AddLine(pos[0], pos[1], lineColor);
		pos[0].x = pos[1].x = -grid;
		Geometory::AddLine(pos[0], pos[1], lineColor);
		pos[0].x = size;
		pos[1].x = -size;
		pos[0].z = pos[1].z = grid;
		Geometory::AddLine(pos[0], pos[1], lineColor);
		pos[0].z = pos[1].z = -grid;
		Geometory::AddLine(pos[0], pos[1], lineColor);
	}
	// 軸
	Geometory::AddLine(DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3(size,0,0), DirectX::XMFLOAT4(1,0,0,1));
	Geometory::AddLine(DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3(0,size,0), DirectX::XMFLOAT4(0,1,0,1));
	Geometory::AddLine(DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3(0,0,size), DirectX::XMFLOAT4(0,0,1,1));
	Geometory::AddLine(DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3(-size,0,0),  DirectX::XMFLOAT4(0,0,0,1));
	Geometory::AddLine(DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3(0,0,-size),  DirectX::XMFLOAT4(0,0,0,1));

	Geometory::DrawLines();

	//// カメラの値
	//static bool camPosSwitch = false;
	//if (IsKeyTrigger(VK_RETURN)) {
	//	camPosSwitch ^= true;
	//}

	//static DirectX::XMVECTOR camPos;
	//if (camPosSwitch) {
	//	camPos = DirectX::XMVectorSet(2.5f, 30.5f, -200.0f, 0.0f);
	//}
	//else {
	//	camPos = DirectX::XMVectorSet(2.5f, 3.5f, -4.0f, 0.0f);
	//}

	//// ジオメトリ用カメラ初期化
	//DirectX::XMFLOAT4X4 mat[2];
	//DirectX::XMStoreFloat4x4(&mat[0], DirectX::XMMatrixTranspose(
	//	DirectX::XMMatrixLookAtLH(
	//		camPos,
	//		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	//	)));
	//DirectX::XMStoreFloat4x4(&mat[1], DirectX::XMMatrixTranspose(
	//	DirectX::XMMatrixPerspectiveFovLH(
	//		DirectX::XMConvertToRadians(60.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f)
	//));
	//Geometory::SetView(mat[0]);
	//Geometory::SetProjection(mat[1]);
#endif
	g_pScene->RootDraw();
//	g_pGame->Draw();
	EndDrawDirectX();
}

void ModelLoad()
{
	ModelCache::GetInstance()->GetCache("Player")->Load("Assets/model/a.fbx",0.015f);
	ModelCache::GetInstance()->GetCache("PlayerEquip")->Load("Assets/model/無題.fbx",1.5f);
	ModelCache::GetInstance()->GetCache("PlayerEquip")->AddAnimation("Assets/model/無題.fbx");
	ModelCache::GetInstance()->GetCache("Cloud")->Load("Assets/model/cloud.fbx",0.2f);
	ModelCache::GetInstance()->GetCache("SkyDome")->Load("Assets/model/sky_dome.fbx",50.0f);
}

// EOF