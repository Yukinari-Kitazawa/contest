#pragma once
#include "GameObject.h"
#include"CameraDebug.h"
class Model;
class SkyDome : public GameObject
{
public:
    SkyDome();
    ~SkyDome();
    void Update();
    void Draw();
	//シーンを選ばずにカメラをセットできるようにテンプレート化
    template<typename T>
	void SetCamera(T* InCamera) { m_pCamera = InCamera; }
    void SetPosition(DirectX::XMFLOAT3 InPos) { m_pos = InPos; }
private:
    Model* m_pModel;
    Camera* m_pCamera;
    DirectX::XMFLOAT3 m_Scale;
};

