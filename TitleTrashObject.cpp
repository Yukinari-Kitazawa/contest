#include "TitleTrashObject.h"
#include "TrashObject.h"
#include"Geometory.h"
#include"ModelCache.h"
#include"Model.h"
#include"ShaderList.h"
TitleTrashObject::TitleTrashObject(DirectX::XMFLOAT3 InitPos)
{
}

TitleTrashObject::~TitleTrashObject()
{
}

void TitleTrashObject::Update()
{
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
