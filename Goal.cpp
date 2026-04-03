#include "Goal.h"
#include"Geometory.h"
#include"Player.h"
#include"Collision.h"
#include"ShaderList.h"
#include"ModelCache.h"
#define GOAL_SCALE (1.5f)
#define GOAL_SCALE_OFFSET (0.1f)
#define GOAL_SCALE_RANGE_RIGHT_FIRST (20.0f)
#define GOAL_SCALE_RANGE_LEFT_FIRST (30.0f)
#define GOAL_SCALE_RANGE_RIGHT_SECOND (30.0f)
#define GOAL_SCALE_RANGE_LEFT_SECOND (40.0f)
#define GOAL_SCALE_RANGE_RIGHT_LAST (40.0f)
Goal::Goal()
{
}

Goal::Goal(DirectX::XMFLOAT3 InitPos):m_pPlayer(nullptr),m_pModel(nullptr),m_pCamera(nullptr),ObjectNum(0)
{
	m_pModel=ModelCache::GetInstance()->GetCache("Cloud");
	m_pos = InitPos;
	Scale = { GOAL_SCALE - GOAL_SCALE_OFFSET * 2,GOAL_SCALE - GOAL_SCALE_OFFSET * 2,GOAL_SCALE - GOAL_SCALE_OFFSET * 2 };//初期化のサイズを小さめ（1.3）に設定
	m_box = { InitPos,Scale };
	m_pSetSe = LoadSound("Assets/sound/Set.mp3");
}

Goal::~Goal()
{
	m_pspeaker = nullptr;
}

void Goal::Update()
{
	//プレイヤーと当たり判定を行う
    if (Collision::Hit(m_box, m_pPlayer->GetCollision()).isHit)
    {
		//プレイヤーがアイテムを持っている場合、ゴールにアイテムの数を加算する
        if (m_pPlayer->GetItemNum() > 0)
        {
			m_pspeaker = PlaySound(m_pSetSe);
			m_pspeaker->SetVolume(1.0f);

			//プレイヤーのアイテムの数をゴールに加算し、プレイヤーのアイテムの数を0にする
			ObjectNum += m_pPlayer->GetItemNum();
            m_pPlayer->SetItemNum(m_pPlayer->GetItemNum() - m_pPlayer->GetItemNum());
        }
    }

	//アイテムの数に応じて当たり判定のサイズを0.1ずつ変化させる
	if ((GOAL_SCALE_RANGE_LEFT_FIRST>ObjectNum)&&(ObjectNum > GOAL_SCALE_RANGE_RIGHT_FIRST))
	{
		Scale = { GOAL_SCALE- GOAL_SCALE_OFFSET,GOAL_SCALE- GOAL_SCALE_OFFSET,GOAL_SCALE- GOAL_SCALE_OFFSET };
		
	}
	else if ((GOAL_SCALE_RANGE_LEFT_SECOND > ObjectNum) && (ObjectNum > GOAL_SCALE_RANGE_RIGHT_SECOND))
	{
		Scale = { GOAL_SCALE,GOAL_SCALE,GOAL_SCALE };

	}
	else if (ObjectNum > GOAL_SCALE_RANGE_RIGHT_LAST)
	{
		Scale = { GOAL_SCALE+ GOAL_SCALE_OFFSET,GOAL_SCALE+ GOAL_SCALE_OFFSET,GOAL_SCALE+ GOAL_SCALE_OFFSET };

	}
	//当たり判定のサイズをアイテムの数に応じて変化させる
	m_box = { m_pos,Scale };
	
}

void Goal::Draw()
{
	RenderTarget* pRTV = GetDefaultRTV(); // RenderTargetView 
	DepthStencil* pDSV = GetDefaultDSV(); // DepthStencilView 
	SetRenderTargets(1, &pRTV, pDSV);  // 3 null 2D表示になる
	DirectX::XMFLOAT4X4 wvp[3];
	DirectX::XMMATRIX T =
		DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	DirectX::XMMATRIX S =
		DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_Rotation.x)) *
		DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(m_Rotation.y)) *
		DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(m_Rotation.z)); // 回転
	DirectX::XMMATRIX mat = S * R * T;
	DirectX::XMStoreFloat4x4(&wvp[0], DirectX::XMMatrixTranspose(mat));
	////頂点シェーダーに渡す変換行列を作成
	DirectX::XMMATRIX world, view, proj;
	//DirectX::XMStoreFloat4x4(&wvp[1], DirectX::XMMatrixTranspose(view));
	//DirectX::XMStoreFloat4x4(&wvp[2], DirectX::XMMatrixTranspose(proj));
	wvp[1] = m_pCamera->GetViewMatrix();
	wvp[2] = m_pCamera->GetProjectionMatrix();

	// シェーダーへの変換行列を設定
	ShaderList::SetWVP(wvp);

	//// モデルに使用する頂点シェーダー、ピクセルシェーダーを設定
	m_pModel->SetVertexShader(ShaderList::GetVS(ShaderList::VS_WORLD));
	m_pModel->SetPixelShader(ShaderList::GetPS(ShaderList::PS_LAMBERT));

	// 複数のメッシュで構成されている場合、ある部分は金属的な表現、ある部分は非金属的な表現と
	// 分ける場合がある。前回の表示は同じマテリアルで一括表示していたため、メッシュごとにマテリアルを
	// 切り替える。


	for (int i = 0; i < m_pModel->GetMeshNum(); ++i) {
		//モデルのメッシュを取得
		Model::Mesh mesh = *m_pModel->GetMesh(i);
		//メッシュに割り当てられているマテリアル取得
		Model::Material material = *m_pModel->GetMaterial(mesh.materialID);
		//シェーダーへマテリアルを設定
		material.ambient = { 0.6,0.6,0.6,1.0f };
		material.diffuse.x = 1.0f;
		material.diffuse.y = 1.0f;
		material.diffuse.z = 5.0f;
		ShaderList::SetMaterial(material);
		//モデルの描画
		m_pModel->Draw(i);
	}
}
