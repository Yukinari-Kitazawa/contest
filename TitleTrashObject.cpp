#include "TitleTrashObject.h"
#include "TrashObject.h"
#include"Geometory.h"
#include"ModelCache.h"
#include"Model.h"
#include"ShaderList.h"

#define WALL_X (-10.0f)

TitleTrashObject::TitleTrashObject(DirectX::XMFLOAT3 InitPos)
{
	m_pos = InitPos;
	m_PrevPos = m_pos;
	m_Velocity = m_pos;
	m_pModel = ModelCache::GetInstance()->GetCache("Cloud");
	m_box = {
		DirectX::XMFLOAT3(m_pos),
		DirectX::XMFLOAT3(2.0f,2.0f,2.0)
	};
	m_Scale = { 2.0f,2.0f,2.0f };
	m_radius = 0.4f;
	m_mass = 1.0f;
}

TitleTrashObject::~TitleTrashObject()
{
}

void TitleTrashObject::Update()
{
	Move();
	m_box.center = m_pos;
}

void TitleTrashObject::Draw()
{
	RenderTarget* pRTV = GetDefaultRTV(); // RenderTargetView 
	DepthStencil* pDSV = GetDefaultDSV(); // DepthStencilView 
	SetRenderTargets(1, &pRTV, pDSV);  // 3 null 2D表示になる
	DirectX::XMFLOAT4X4 wvp[3];
	DirectX::XMMATRIX T =
		DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	DirectX::XMMATRIX S =
		DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
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

	

	for (int i = 0; i < m_pModel->GetMeshNum(); ++i) {
		//モデルのメッシュを取得
		const Model::Mesh* mesh = m_pModel->GetMesh(i);
		//メッシュに割り当てられているマテリアル取得
		Model::Material material = *m_pModel->GetMaterial(mesh->materialID);
		//シェーダーへマテリアルを設定
		material.ambient = { 0.8f,0.8f,0.8f,1.0f };
		ShaderList::SetMaterial(material);
		//モデルの描画
		m_pModel->Draw(i);
	}
}

void TitleTrashObject::Move()
{
	m_PrevPos = m_pos;

	m_pos.x += m_Velocity.x*0.005f;
	m_pos.y += m_Velocity.y*0.005f;
	
	// 壁との反射
	const float minX = -15.0f + m_radius;
	const float maxX = 15.0f - m_radius;
	const float minY = -8.0f + m_radius;
	const float maxY = 8.5f - m_radius;
	const float restitution = 1.0f;  // 反発係数（1.0f だと減速しない）

	// 左右
	if (m_pos.x < minX) {
		m_pos.x = minX;
		m_Velocity.x *= -restitution;
	}
	else if (m_pos.x > maxX) {
		m_pos.x = maxX;
		m_Velocity.x *= -restitution;
	}

	// 上下
	if (m_pos.y < minY) {
		m_pos.y = minY;
		m_Velocity.y *= -restitution;
	}
	else if (m_pos.y > maxY) {
		m_pos.y = maxY;
		m_Velocity.y *= -restitution;
	}
}

bool TitleTrashObject::ResolveCollision(TitleTrashObject& a, TitleTrashObject& b, float E)
{
	// ここでは X-Y 平面上の円同士として扱う（Z は無視）
	float dx = b.m_pos.x - a.m_pos.x;
	float dy = b.m_pos.y - a.m_pos.y;

	float dist2 = dx * dx + dy * dy;
	float minDist = a.m_radius + b.m_radius;

	// 離れているなら何もしない
	if (dist2 == 0.0f) return false;
	if (dist2 > minDist * minDist) return false;

	float dist = std::sqrt(dist2);

	// 衝突の法線ベクトル（a → b）
	float nx = dx / dist;
	float ny = dy / dist;

	// a,b の速度を法線方向に投影
	float v1 = a.m_Velocity.x * nx + a.m_Velocity.y * ny;
	float v2 = b.m_Velocity.x * nx + b.m_Velocity.y * ny;

	// すでに離れていっているならスキップ（なくても動くけど安定性UP）
	if (v1 - v2 < 0.0f) {
		return false;
	}

	// 接線方向成分（そのまま残す）
	float v1tx = a.m_Velocity.x - v1 * nx;
	float v1ty = a.m_Velocity.y - v1 * ny;
	float v2tx = b.m_Velocity.x - v2 * nx;
	float v2ty = b.m_Velocity.y - v2 * ny;

	float m1 = a.m_mass;
	float m2 = b.m_mass;

	// 1 次元の反発をもつ衝突公式（法線方向のスカラー速度だけ処理）
	float v1d = ((m1 - E * m2) * v1 + (1.0f + E) * m2 * v2) / (m1 + m2);
	float v2d = ((m2 - E * m1) * v2 + (1.0f + E) * m1 * v1) / (m1 + m2);

	// 新しい速度 = 接線成分 + 法線方向の成分
	a.m_Velocity.x = v1tx + v1d * nx;
	a.m_Velocity.y = v1ty + v1d * ny;

	b.m_Velocity.x = v2tx + v2d * nx;
	b.m_Velocity.y = v2ty + v2d * ny;

	// --------------------------
	// めり込み解消（半分ずつ押し戻す）
	// --------------------------
	float overlap = minDist - dist;
	if (overlap > 0.0f) {
		a.m_pos.x -= nx * overlap * 0.5f;
		a.m_pos.y -= ny * overlap * 0.5f;
		b.m_pos.x += nx * overlap * 0.5f;
		b.m_pos.y += ny * overlap * 0.5f;
	}
	return true;
}
