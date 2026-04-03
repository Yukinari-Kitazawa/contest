#include "StageObjectManager.h"
#include "TrashObject.h"
#include <random>
#include"Collision.h"
#include"Player.h"
#include"Wall.h"
#include"Goal.h"
#include"ModelCache.h"
#include"Model.h"
#include"CameraPlayer.h"
#include"SkyDome.h"
#define STAGE_OBJECT_NUMBER (40)
#define GOAL_INIT_POSITION DirectX::XMFLOAT3(0.0f, 5.0f, 0.0f)
#define MAX_ITEM_NUM (5)
StageObjectManager::StageObjectManager():
	m_pPlayer(nullptr),
	m_pGoal(nullptr)
{
	if (m_pPlayer)
	{
		MessageBox(NULL, "StageObjectManagerにプレイヤーがセットされていません", "エラー", MB_OK);
	}
	//スカイドームの設置
	m_pSkyDome = new SkyDome();


	//=====ゴールの配置==========
	m_pGoal = new Goal(GOAL_INIT_POSITION);
	

	//=======オブジェクトの配置========
	std::random_device Rd;//メルセンヌツイスタ関数を使ってランダムにオブジェクトを配置
	std::mt19937 Gen(Rd());
	std::uniform_real_distribution<float> DistXZ(-10.0f, 10.0f);//XZ方向は-10.0f～10.0fの範囲
	std::uniform_real_distribution<float> DistY(0.3f, 10.0f); //Y方向は - 10.0f～10.0fの範囲
	for (int i = 0; i < STAGE_OBJECT_NUMBER; i++)
	{
		while (Collision::Hit(Collision::Box{ {0.0f,5.0f,0.0f}, {1.5f,1.5f,1.5f} }, { {DistXZ(Gen),DistY(Gen),DistXZ(Gen)}, {1.5f,1.5f,1.5f} }).isHit)//スタート位置に出現しないようにする
		{
			std::uniform_real_distribution<float> DistXZ(-10.0f, 10.0f);//XZ方向は-10.0f～10.0fの範囲
			std::uniform_real_distribution<float> DistY(0.0f, 10.0f); //Y方向は - 10.0f～10.0fの範囲
		}
		m_StageObjects.push_back(new TrashObject({ DistXZ(Gen),DistY(Gen),DistXZ(Gen) }, DistY(Gen)));//ランダムな座標に領域確保
	}

	//=======壁の配置===========
	struct WallIndex//壁の位置、回転、サイズの情報
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Rotation;
		DirectX::XMFLOAT3 Size;
	};
	WallIndex wallindex[6];//ボックス上に当たり判定
	wallindex[WALL_UP] = { {0.0f,WALL_POSITION,0.0f},{0.0f,0.0f,0.0f},{20.0f,1.0f,20.0f} };//上
	wallindex[WALL_DOWN] = { {0.0f,-0.5f,0.0f},{0.0f,0.0f,0.0f},{20.0f,1.0f,20.0f} };//下
	wallindex[WALL_LEFT] = { {-WALL_POSITION,0.0f,0.0f},{0.0f,0.0f,0.0f},{1.0f,20.0f,20.0f} };//左
	wallindex[WALL_RIGHT] = { {WALL_POSITION,0.0f,0.0f},{0.0f,0.0f,0.0f},{1.0f,20.0f,20.0f} };//右
	wallindex[WALL_FRONT] = { {0.0f,5.0f,WALL_POSITION},{0.0f,0.0f,0.0f},{20.0f,10.0f,1.0f} };//前
	wallindex[WALL_BACK] = { {0.0f,0.0f,-WALL_POSITION},{0.0f,0.0f,0.0f},{20.0f,20.0f,1.0f} };//後ろ

	for (int i = 0; i < WALL_NUM; i++)
	{
		m_WallObjects.push_back(new Wall(wallindex[i].Pos, wallindex[i].Rotation, wallindex[i].Size,i));
	}
	//サウンド関係
	m_pGetSe = LoadSound("Assets/sound/Get.mp3");

}

StageObjectManager::~StageObjectManager()
{
	for (auto& itr : m_StageObjects)//範囲for文でdelete
	{
		if (!itr)continue;//オブジェクトがあるかどうかチェック
		delete itr;
		itr = nullptr;
	}
	m_StageObjects.clear();
	for (auto& itr : m_WallObjects)//範囲for文でdelete
	{
		if (!itr)continue;//オブジェクトがあるかどうかチェック
		delete itr;
		itr = nullptr;
	}
	m_WallObjects.clear();
	SAFE_DELETE(m_pGoal);
	SAFE_DELETE(m_pSkyDome);
}

void StageObjectManager::Update()
{
	m_pGoal->SetPlayer(m_pPlayer);
	for (auto itr = m_StageObjects.begin(); itr != m_StageObjects.end(); )
	{
		if (!(*itr)) {
			++itr; // nullptrの場合はスキップ
			continue;
		}

		(*itr)->Update();

		if (Collision::Hit((*itr)->GetCollision(), m_pPlayer->GetCollision()).isHit) {
			// プレイヤーとオブジェクトが当たった場合
			if (m_pPlayer->GetItemNum() < MAX_ITEM_NUM) {
				if (!(*itr)->GetDeletePrepareFlag())
				{
					m_pPlayer->SetItemNum(m_pPlayer->GetItemNum() + 1);
					m_pSEspeaker = PlaySound(m_pGetSe);
					m_pSEspeaker->SetVolume(1.0f);
					// 現在の要素を削除し、イテレーターを次に進める
					(*itr)->SetDeletePrepareFlag(true);
					continue;
				}
				
			}
		}
		if ((*itr)->GetDeleteFlag())
		{
			itr = m_StageObjects.erase(itr);
			continue;
		}
		++itr; // 通常時は次の要素に進む
	}
	for (auto& itr : m_WallObjects)
	{
		if (!itr)continue;//オブジェクトがあるかどうかチェック
		itr->Update();
	}
	m_pGoal->Update();
	m_pSkyDome->SetPosition(m_pPlayer->GetPos());
	m_pSkyDome->Update();
}

void StageObjectManager::Draw()
{
	for (auto& itr : m_StageObjects)
	{
		if (!itr)continue;//オブジェクトがあるかどうかチェック
		itr->Draw();
	}
	m_pSkyDome->Draw();
	m_pGoal->Draw();
}
//カメラの設定
void StageObjectManager::SetCamera(CameraPlayer* InCamera)
{
	m_pCamera = InCamera;
	m_pGoal->SetCamera(InCamera);
	m_pSkyDome->SetCamera(InCamera);
	for (auto& itr : m_StageObjects)
	{
		itr->SetCamera(InCamera);
	}
}

