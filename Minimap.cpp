#include "Minimap.h"
#define MINIMAP_SIZE (512)
Minimap::Minimap() : m_pRenderTarget(nullptr) 
{
    //作成する書き込み先の大きさ
    float width = MINIMAP_SIZE;
    float height = MINIMAP_SIZE;

    // レンダーターゲットを作成
    m_pRenderTarget = new RenderTarget();
    m_pRenderTarget->Create(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
    
    // 深度ステンシルを作成
    m_pDepthStencil = new DepthStencil();
    m_pDepthStencil->Create(width, height, false);
}
Minimap::~Minimap() 
{
    if (m_pDepthStencil) {
        delete m_pDepthStencil;
        m_pDepthStencil = nullptr;
    }
    if (m_pRenderTarget) {
        delete m_pRenderTarget;
        m_pRenderTarget = nullptr;
    }
}
///ミニマップを描画
void Minimap::Draw() 
{
    DirectX::XMFLOAT4X4 world, view, proj;

    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(490.0f, 210.0f, 0.0f);
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);// Y上下反転

    DirectX::XMMATRIX mWorld = S * T;// 拡縮→回転→移動
    mWorld = DirectX::XMMatrixTranspose(mWorld);// 転置
    DirectX::XMStoreFloat4x4(&world, mWorld);

    //画面の手前に引いてカメラを設置
    DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH(
        DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),//カメラの位置
        DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), //カメラの注視点
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)//カメラのUpベクトル
    );

    //2Dに見える行列を設定。原点の位置をどこにするか
    //考えながら、各引数を指定
    DirectX::XMMATRIX mProj = DirectX::XMMatrixOrthographicOffCenterLH(
        -640.0f, 640.0f, -360.0f, 360.0f, 0.5f, 100.0f
    );
    mView = DirectX::XMMatrixTranspose(mView);
    mProj = DirectX::XMMatrixTranspose(mProj);
    DirectX::XMStoreFloat4x4(&view, mView);
    DirectX::XMStoreFloat4x4(&proj, mProj);
    Sprite::SetView(view);
    Sprite::SetProjection(proj);
    //画面中央を原点として、右上あたりに表示
    //画面幅(1280,720)とすると画面上と画面右はそれぞれ(640,360)となる。画面サイズは200で
    //画面端から50離して表示したいので

    //スプライトに各種情報を設定して表示
    Sprite::SetWorld(world);
    // ミニマップのサイズと色を設定
    Sprite::SetSize({ 200.0f, 200.0f }); // 画面サイズ
    Sprite::SetColor({ 1, 1, 1, 1 });
    Sprite::SetUVPos({0.0f,0.0f});
    Sprite::SetUVScale({1.0f,1.0f});
    Sprite::SetTexture(m_pRenderTarget);
    Sprite::Draw();
}

///ミニマップの作成開始
void Minimap::BeginRender() 
{
    // レンダーターゲットと深度ステンシルをクリア
    m_pRenderTarget->Clear();
    m_pDepthStencil->Clear();

    // ミニマップのレンダーターゲットを設定
    SetRenderTargets(1, &m_pRenderTarget, m_pDepthStencil);
}
///ミニマップの作成終了
void Minimap::EndRender() 
{
        // デフォルトのレンダーターゲットに戻す
        RenderTarget* pDefRTV = GetDefaultRTV();
        DepthStencil* pDefDSV = GetDefaultDSV();
        SetRenderTargets(1, &pDefRTV, pDefDSV);
}

