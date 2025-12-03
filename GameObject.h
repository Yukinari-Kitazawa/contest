#pragma once
#include<DirectXMath.h>
#include"Collision.h"
class GameObject
{
public:
	GameObject();
	GameObject(DirectX::XMFLOAT3 InitPos);
	virtual ~GameObject();
	virtual void Update();
	virtual void Draw();
	DirectX::XMFLOAT3 GetPos() { return m_pos; }
	DirectX::XMFLOAT3 GetRotation() { return m_Rotation; }
	Collision::Box GetCollision() {
		return m_box;
	}
	void SetRotateY(float InRotate) { m_Rotation.y = InRotate; }
protected:
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_Rotation;
	Collision::Box m_box;
};

