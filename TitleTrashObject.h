#pragma once
#include "GameObject.h"
#include"ModelCache.h"
#include"CameraTitle.h"

class TitleTrashObject : public GameObject
{
public:
	TitleTrashObject(DirectX::XMFLOAT3 InitPos);
	~TitleTrashObject();
	void Update() override;
	void Draw() override;
private:
	Model* m_pModel;
	DirectX::XMFLOAT3 m_Scale;
	CameraTitle* m_pCamera;


};

