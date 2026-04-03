#include "CameraMinimap.h"

CameraMinimap::CameraMinimap() : m_pPlayer(nullptr) {
    m_aspect = 1.0f;
}

CameraMinimap::~CameraMinimap()
{
}

void CameraMinimap::Update() {
    //プレイヤーが設定されているか確認
    if (!m_pPlayer) { return; }
    //プレイヤーの上空にカメラを設定
    m_look = m_pPlayer->GetPos();
    m_pos = m_look;
    m_pos.y += 5.0f; 

    // 真上から見下ろすためアップベクトルはZ方向へ向ける
    m_up = { 0.0f, 0.0f, 1.0f };
}