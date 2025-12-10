#include <DirectXMath.h>
#include"Defines.h"
using namespace DirectX;


// ワールド座標をスクリーン座標(ピクセル)に変換する
static XMFLOAT3 WorldToScreen(
    const XMFLOAT3& worldPos,
    const XMFLOAT4X4& view,
    const XMFLOAT4X4& proj)
{
    XMMATRIX mView = XMLoadFloat4x4(&view);
    XMMATRIX mProj = XMLoadFloat4x4(&proj);
    XMMATRIX vp = XMMatrixMultiply(mView, mProj);

    XMVECTOR pos = XMLoadFloat3(&worldPos);
    // view * proj で変換 & w除算までやってくれる
    XMVECTOR ndc = XMVector3TransformCoord(pos, vp);

    XMFLOAT3 ndcF;
    XMStoreFloat3(&ndcF, ndc); // ndcF.x, ndcF.y ∈ [-1, 1]

    XMFLOAT3 screen;
    screen.x = (ndcF.x * 0.5f + 0.5f) * SCREEN_WIDTH;
    screen.y = (-ndcF.y * 0.5f + 0.5f) * SCREEN_HEIGHT; // Y反転
    screen.z = 0.0f;
    return screen;
}