#include "CameraResult.h"
using namespace DirectX;
CameraResult::CameraResult() : m_radXZ(0.0f), m_radY(15.0f), m_radius(4.0f)
{
}

CameraResult::~CameraResult()
{
}

void CameraResult::Update()
{
	//------デバッグ用操作------
	//if (IsKeyPress(VK_UP)) { m_look.z += 1.0f; }
	//if (IsKeyPress(VK_DOWN)) { m_look.z += -1.0f; }
	//if (IsKeyPress(VK_LEFT)) { m_look.x += -1.0f; }
	//if (IsKeyPress(VK_RIGHT)) { m_look.x += 1.0f; }
	//if (IsKeyPress(VK_SHIFT)) { m_look.y += 1.0f; }
	//if (IsKeyPress(VK_CONTROL)) { m_look.y += -1.0f; }
	//if (IsKeyPress('A')) { m_radXZ += 1.0f; }
	//if (IsKeyPress('D')) { m_radXZ += -1.0f; }
	//if (IsKeyPress('W')) { m_radY += -1.0f; }
	//if (IsKeyPress('S')) { m_radY += 1.0f; }
	//if (IsKeyPress('Q')) { m_radius += -1.0f; }
	//if (IsKeyPress('E')) { m_radius += 1.0f; }
	//m_pos.x = cos(DirectX::XMConvertToRadians(m_radY)) * sin(DirectX::XMConvertToRadians(m_radXZ)) * m_radius + m_look.x;
	//m_pos.y = sin(DirectX::XMConvertToRadians(m_radY)) * m_radius + m_look.y;
	//m_pos.z = cos(DirectX::XMConvertToRadians(m_radY)) * cos(DirectX::XMConvertToRadians(m_radXZ)) * m_radius + m_look.z;
	//-----------------------
	if (m_radius < 0.1f) m_radius = 0.1f;//0はエラー

}
