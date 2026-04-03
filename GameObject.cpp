#include "GameObject.h"
#include "Geometory.h"

//基底クラスのため、メンバ変数の初期化は最低限にしている
GameObject::GameObject() : m_pos(0.0f, 0.0f, 0.0f),m_Rotation(0.0f,0.0f,0.0f)
{

}

GameObject::GameObject(DirectX::XMFLOAT3 InitPos):m_pos(InitPos), m_Rotation(0.0f, 0.0f, 0.0f)
{
	
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
}

void GameObject::Draw()
{

}
