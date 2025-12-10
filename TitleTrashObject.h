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
	void Move();
	bool ResolveCollision(TitleTrashObject& a, TitleTrashObject& b, float restitution = 0.9f);
	void SetCamera(Camera* InCamera) { m_pCamera = static_cast<CameraTitle*>(InCamera); }
protected:
	Model* m_pModel;
private:
	DirectX::XMFLOAT3 m_Scale;
	DirectX::XMFLOAT3 m_Velocity;
	CameraTitle* m_pCamera;
	float             m_radius;      // ”¼Œa
	DirectX::XMFLOAT3 m_PrevPos;
	float m_mass = 1.0f;   // Ž¿—Ê
};

