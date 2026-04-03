#include "Player.h"
#include"Defines.h"
#include"GaugeUI.h"
#include"ShaderList.h"
#include"ModelCache.h"
//=============定数定義=========================
#define MOVE_SPEED_BASE (0.07f)//スピードのベース
#define MOVE_SPEED_NORMAl (0.0f)//通常のスピード
#define MOVE_SPEED_DASH (0.05f)//ダッシュのスピード
#define MOVE_SPEED_SLOW (-0.06f)//減速時のスピード
#define ROTATION_SPEED_BASE (0.9f)//回転のスピードベース
#define DASH_TIME (60)//ダッシュする時間
#define DASH_INTERVAL (120)//ダッシュができるまでの時間
#define GAUGE_UI_CORRECT (0.0054f)
#define COLLISION_CORRECT (0.001f)
#define PLAYER_UI_SPRITE_SPRIT_X (5.0f)
#define PLAYER_UI_SPRITE_SPRIT_Y (2.0f)
#define PLAYER_UI_SPRITE_POS_X (80.0f)
#define PLAYER_UI_SPRITE_POS_Y (80.0f)
#define PLAYER_EFFECT_SPRITE_SIZE_X (1280.0f)
#define PLAYER_EFFECT_SPRITE_SIZE_Y (850.0f)
#define PLAYER_EFFECT_SPRITE_SPRIT_X (6.0f)
#define PLAYER_EFFECT_SPRITE_SPRIT_Y (6.0f)
#define PLAYER_EFFECT_SPRITE_POS_X (640)
#define PLAYER_EFFECT_SPRITE_POS_Y (300)
//============グロ－バル定義====================

enum ePlayerState {//プレイヤーの状態
    NORMAL,//通常
    DASH,//ダッシュ
    SLOW//減速
};
ePlayerState g_eState;
Player::Player() :
    m_pCamera(nullptr),
    m_move(0.0f, 0.0f, 0.0f),
    m_power(0.0f),
    m_fDashSpeed(0.0f),
    m_nDashIntervalCnt(0),
    m_bDashFlag(false),
    m_pGaugeUI(nullptr),
    m_nGaugeUICnt(DASH_TIME + DASH_INTERVAL),
    m_nItemCnt(0), m_pModel(nullptr)
{
    m_pTexture = new Texture();
    m_pBackTexture = new Texture();
    m_pEffectTexture = new Texture();
    m_pTexture->Create("Assets/texture/number.png");
    m_pBackTexture->Create("Assets/texture/CloudUI.png");
    m_pEffectTexture->Create("Assets/texture/Dash.png");
    m_pModel = ModelCache::GetInstance()->GetCache("Player");
    m_pModelEquip = ModelCache::GetInstance()->GetCache("PlayerEquip");
    m_pos = { 0.0f,0.5f,0.0f };
    m_pGaugeUI = new GaugeUI();
    m_box = {
    DirectX::XMFLOAT3(0.0f,0.5f,0.0f),
    DirectX::XMFLOAT3(1.0f,1.0f,1.0f)
    };
    m_pDashSe = LoadSound("Assets/sound/Dash.mp3");
}

Player::~Player()
{
    m_pspeaker = nullptr;
    SAFE_DELETE(m_pGaugeUI);
    SAFE_DELETE(m_pTexture);
    SAFE_DELETE(m_pBackTexture);
    SAFE_DELETE(m_pEffectTexture);
}
//更新処理
void Player::Update()
{
    //カメラが設定されていない場合は処理しない
    if (!m_pCamera) { return; }
    UpdateMove();

    m_box.center = m_pos;
    OnCollisionWall();

}
//描画処理
void Player::Draw()
{
    RenderTarget* pRTV = GetDefaultRTV();// ディスプレイ情報の取得
    DepthStencil* pDSV = GetDefaultDSV(); // 深度バッファの取得
    // 2D表示の設定 
    SetRenderTargets(1, &pRTV, pDSV);
    DrawUI();
    m_pGaugeUI->Draw();

    DirectX::XMFLOAT4X4 wvp[3];
    DirectX::XMMATRIX T =
        DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
    DirectX::XMMATRIX S =
        DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_Rotation.x)) *
        DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(m_Rotation.z)) *
        DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(m_Rotation.y)); // 回転
    DirectX::XMMATRIX mat = S * R * T;
    DirectX::XMStoreFloat4x4(&wvp[0], DirectX::XMMatrixTranspose(mat));
    wvp[1] = m_pCamera->GetViewMatrix();
    wvp[2] = m_pCamera->GetProjectionMatrix();

    // シェーダーへの変換行列を設定
    ShaderList::SetWVP(wvp);

    //// モデルに使用する頂点シェーダー、ピクセルシェーダーを設定
    m_pModel->SetVertexShader(ShaderList::GetVS(ShaderList::VS_WORLD));
    m_pModel->SetPixelShader(ShaderList::GetPS(ShaderList::PS_LAMBERT));

    // 複数のメッシュで構成されている場合、ある部分は金属的な表現、ある部分は非金属的な表現と
    // 分ける場合がある。前回の表示は同じマテリアルで一括表示していたため、メッシュごとにマテリアルを
    // 切り替える。

    for (int i = 0; i < m_pModel->GetMeshNum(); ++i) {
        //モデルのメッシュを取得
        Model::Mesh mesh = *m_pModel->GetMesh(i);
        //メッシュに割り当てられているマテリアル取得
        Model::Material material = *m_pModel->GetMaterial(mesh.materialID);
        //シェーダーへマテリアルを設定
        material.ambient = { 0.6,0.6,0.6,1.0f };
        ShaderList::SetMaterial(material);
        //モデルの描画
        m_pModel->Draw(i);
    }
    
    T =
       DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
    S =
       DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    R = DirectX::XMMatrixRotationQuaternion(m_Quaternion);

    mat = S * R * T;
    DirectX::XMStoreFloat4x4(&wvp[0], DirectX::XMMatrixTranspose(mat));
    wvp[1] = m_pCamera->GetViewMatrix();
    wvp[2] = m_pCamera->GetProjectionMatrix();

    // シェーダーへの変換行列を設定
    ShaderList::SetWVP(wvp);

    //// モデルに使用する頂点シェーダー、ピクセルシェーダーを設定
    m_pModelEquip->SetVertexShader(ShaderList::GetVS(ShaderList::VS_WORLD));
    m_pModelEquip->SetPixelShader(ShaderList::GetPS(ShaderList::PS_LAMBERT));

    // 複数のメッシュで構成されている場合、ある部分は金属的な表現、ある部分は非金属的な表現と
    // 分ける場合がある。前回の表示は同じマテリアルで一括表示していたため、メッシュごとにマテリアルを
    // 切り替える。

    for (int i = 0; i < m_pModelEquip->GetMeshNum(); ++i) {
        //モデルのメッシュを取得
        Model::Mesh mesh = *m_pModelEquip->GetMesh(i);
        //メッシュに割り当てられているマテリアル取得
        Model::Material material = *m_pModelEquip->GetMaterial(mesh.materialID);
        //シェーダーへマテリアルを設定
        material.ambient = { 0.6,0.6,0.6,1.0f };
        ShaderList::SetMaterial(material);
        //モデルの描画
        m_pModelEquip->Draw(i);
    }
    if (g_eState == DASH) {//ダッシュ状態のときエフェクトを出す
        DrawEffect();
    }
    
}

void Player::DrawMiniMapModel()
{
    DirectX::XMFLOAT4X4 wvp[3];
    DirectX::XMMATRIX T =
        DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
    DirectX::XMMATRIX S =
        DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_Rotation.x)) *
        DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(m_Rotation.z)) *
        DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(m_Rotation.y)); // 回転
    DirectX::XMMATRIX mat = S * R * T;
    DirectX::XMStoreFloat4x4(&wvp[0], DirectX::XMMatrixTranspose(mat));
    wvp[1] = m_pCamera->GetViewMatrix();
    wvp[2] = m_pCamera->GetProjectionMatrix();

    // シェーダーへの変換行列を設定
    ShaderList::SetWVP(wvp);

    //// モデルに使用する頂点シェーダー、ピクセルシェーダーを設定
    m_pModel->SetVertexShader(ShaderList::GetVS(ShaderList::VS_WORLD));
    m_pModel->SetPixelShader(ShaderList::GetPS(ShaderList::PS_LAMBERT));

    // 複数のメッシュで構成されている場合、ある部分は金属的な表現、ある部分は非金属的な表現と
    // 分ける場合がある。前回の表示は同じマテリアルで一括表示していたため、メッシュごとにマテリアルを
    // 切り替える。

    for (int i = 0; i < m_pModel->GetMeshNum(); ++i) {
        //モデルのメッシュを取得
        Model::Mesh mesh = *m_pModel->GetMesh(i);
        //メッシュに割り当てられているマテリアル取得
        Model::Material material = *m_pModel->GetMaterial(mesh.materialID);
        //シェーダーへマテリアルを設定
        material.ambient = { 0.6,0.6,0.6,1.0f };
        ShaderList::SetMaterial(material);
        //モデルの描画
        m_pModel->Draw(i);
    }

    T =
        DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
    S =
        DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    R = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_Rotation.x)) *
        DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(m_Rotation.z)) *
        DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(m_Rotation.y)); // 回転
    mat = S * R * T;
    DirectX::XMStoreFloat4x4(&wvp[0], DirectX::XMMatrixTranspose(mat));
    wvp[1] = m_pCamera->GetViewMatrix();
    wvp[2] = m_pCamera->GetProjectionMatrix();

    // シェーダーへの変換行列を設定
    ShaderList::SetWVP(wvp);

    //// モデルに使用する頂点シェーダー、ピクセルシェーダーを設定
    m_pModelEquip->SetVertexShader(ShaderList::GetVS(ShaderList::VS_WORLD));
    m_pModelEquip->SetPixelShader(ShaderList::GetPS(ShaderList::PS_LAMBERT));

    // 複数のメッシュで構成されている場合、ある部分は金属的な表現、ある部分は非金属的な表現と
    // 分ける場合がある。前回の表示は同じマテリアルで一括表示していたため、メッシュごとにマテリアルを
    // 切り替える。

    for (int i = 0; i < m_pModelEquip->GetMeshNum(); ++i) {
        //モデルのメッシュを取得
        Model::Mesh mesh = *m_pModelEquip->GetMesh(i);
        //メッシュに割り当てられているマテリアル取得
        Model::Material material = *m_pModelEquip->GetMaterial(mesh.materialID);
        //シェーダーへマテリアルを設定
        material.ambient = { 0.6,0.6,0.6,1.0f };
        ShaderList::SetMaterial(material);
        //モデルの描画
        m_pModelEquip->Draw(i);
    }
}
void Player::UpdateMove()
{
	//移動処理
    if (IsKeyPress('A'))
    {
        m_Rotation.z += -ROTATION_SPEED_BASE;
    }
    if (IsKeyPress('D'))
    {
        m_Rotation.z += ROTATION_SPEED_BASE;
    }
    if (IsKeyPress('W'))
    {
        m_Rotation.x += -ROTATION_SPEED_BASE;
    }
    if (IsKeyPress('S'))
    {
        m_Rotation.x += ROTATION_SPEED_BASE;
    }
    //キャラクターの状態変更(基本はNORMAL状態)
    g_eState = NORMAL;
    if (IsKeyTrigger(VK_SHIFT))//ダッシュフラグを立てる
    {
        if (!m_bDashFlag) {
            m_pspeaker = PlaySound(m_pDashSe);
            m_pspeaker->SetVolume(1.0f);
            m_bDashFlag = true;
            m_nGaugeUICnt = 0;
        }
        
    }
    if (m_bDashFlag)//ダッシュフラグが立っていたら
    {
        
        m_nDashIntervalCnt++;
        m_nGaugeUICnt++;
        if (m_nDashIntervalCnt < DASH_TIME)//DASH_TIMEの時間分DASH状態になりスピードが上がる
        {
            g_eState = DASH;
           
        }
        else if(DASH_TIME < m_nDashIntervalCnt && m_nDashIntervalCnt <DASH_TIME + DASH_INTERVAL)//時間が経過したらNORMAL状態へ
        {
            g_eState = NORMAL;
        }
        else if (DASH_TIME + DASH_INTERVAL < m_nDashIntervalCnt)//DASH_INTERVAL分ダッシュを使えなくする
        {
            m_nDashIntervalCnt = 0;//ダッシュフラグを下ろしてカウンタもゼロに
            m_bDashFlag = false;
        }
    }
    if (IsKeyPress(VK_SPACE))//減速
    {
        g_eState = SLOW;
    }
    switch (g_eState)//プレイヤーの状態によって移動スピードを変える
    {
    case NORMAL:
        m_fDashSpeed = MOVE_SPEED_NORMAl;//ノーマルは0.0f
        break;
    case DASH:
        m_fDashSpeed = MOVE_SPEED_DASH;
        break;
    case SLOW:
        m_fDashSpeed = MOVE_SPEED_SLOW;
        break;
    default:
        break;
    }
    m_pGaugeUI->SetGauge(static_cast<float>(m_nGaugeUICnt) * GAUGE_UI_CORRECT);
    m_pGaugeUI->Update();
    //移動関連の更新
    float pitch = DirectX::XMConvertToRadians(m_Rotation.x);
    float yaw = DirectX::XMConvertToRadians(m_Rotation.z);
    float roll = DirectX::XMConvertToRadians(m_Rotation.y);

    // クォータニオンを作る
    m_Quaternion = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

    // クォータニオン → 回転行列へ変換（これによりジンバルロック回避）
    DirectX::XMMATRIX rotMatrix =
        DirectX::XMMatrixRotationQuaternion(m_Quaternion);

    // 行列の z軸方向(row[2]) が forward になる
    DirectX::XMVECTOR forward = rotMatrix.r[2]; // r[2] は (x, y, "z", w)
    // 移動量+
    bool HitWallFlag = false;
    DirectX::XMVECTOR moveVec = DirectX::XMVectorScale(forward, MOVE_SPEED_BASE + m_fDashSpeed);
    if (HitWallFlag)moveVec = DirectX::XMVectorScale(forward, 0.0f);
    DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&m_pos);
    // 位置を更新
    posVec = DirectX::XMVectorAdd(posVec, moveVec);
    DirectX::XMStoreFloat3(&m_pos, posVec);
}
//UIの描画
void Player::DrawUI()
{
    RenderTarget* pRTV = GetDefaultRTV();// ディスプレイ情報の取得
    DepthStencil* pDSV = GetDefaultDSV(); // 深度バッファの取得
    // 2D表示の設定 
    SetRenderTargets(1, &pRTV, nullptr);
    // スプライトの表示に必要な行列を計算 
    DirectX::XMFLOAT4X4 world, view, proj;
    DirectX::XMMATRIX mView =
        DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),
            DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
            DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
        );
    DirectX::XMMATRIX mProj = DirectX::XMMatrixOrthographicOffCenterLH(
        0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0.0f, CMETER(30.0f), METER(2.0f));
    DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixTranspose(mView));
    DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixTranspose(mProj));
    Sprite::SetView(view);
    Sprite::SetProjection(proj);
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(PLAYER_UI_SPRITE_POS_X, PLAYER_UI_SPRITE_POS_Y, 0.0f);
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1.2f, -1.2f, 1.0f);// Y上下反転
    DirectX::XMMATRIX mWorld = S * T;// 拡縮→回転→移動
    mWorld = DirectX::XMMatrixTranspose(mWorld);// 転置
    DirectX::XMStoreFloat4x4(&world, mWorld);
    Sprite::SetWorld(world);
    Sprite::SetSize({ 160.0f, 160.0f }); //　サイズ
    Sprite::SetOffset({ 0.0f, 0.0f });// 座標
    Sprite::SetUVPos({0.0f,0.0f});
    Sprite::SetUVScale({ 1.0f,1.0f});
    Sprite::SetTexture(m_pBackTexture);
    Sprite::SetColor({ 1.0f, 1.0f/ (static_cast<float>(m_nItemCnt+1.0f)*0.6f), 1.0f / (static_cast<float>(m_nItemCnt + 1.0f) * 0.6f), 1.0f });
    Sprite::Draw();  

    //一の位
    int one = m_nItemCnt % 10;
    T = DirectX::XMMatrixTranslation(PLAYER_UI_SPRITE_POS_X-5.0f, PLAYER_UI_SPRITE_POS_Y+12.0f, 0.0f);
    S = DirectX::XMMatrixScaling(1.0f, -1.0f, 1.0f);// Y上下反転
    mWorld = S * T;// 拡縮→回転→移動
    mWorld = DirectX::XMMatrixTranspose(mWorld);// 転置
    DirectX::XMStoreFloat4x4(&world, mWorld);
    Sprite::SetWorld(world);
    Sprite::SetSize({ 160.0f, 160.0f }); //　サイズ
    Sprite::SetOffset({ 0.0f, 0.0f });// 座標
    Sprite::SetUVPos({ 1.0f / PLAYER_UI_SPRITE_SPRIT_X * (one % static_cast<int>(PLAYER_UI_SPRITE_SPRIT_X)),1.0f / PLAYER_UI_SPRITE_SPRIT_Y * (one / static_cast<int>(PLAYER_UI_SPRITE_SPRIT_X)) });
    Sprite::SetUVScale({ 1.0f / PLAYER_UI_SPRITE_SPRIT_X,1.0f / PLAYER_UI_SPRITE_SPRIT_Y });
    Sprite::SetTexture(m_pTexture);
    Sprite::SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    Sprite::Draw();
}

//ダッシュエフェクトの描画
void Player::DrawEffect()
{
    RenderTarget* pRTV = GetDefaultRTV();// ディスプレイ情報の取得
    // 2D表示の設定 
    SetRenderTargets(1, &pRTV, nullptr);
    // スプライトの表示に必要な行列を計算 
    static int cnt = 0;
    static int FrameCnt = 0;
	//フレームカウントが5を超えたら次の画像に切り替える
    if (FrameCnt > 4)
    {
        cnt++;
        FrameCnt = 0;
    }
    FrameCnt++;
    if (cnt > PLAYER_EFFECT_SPRITE_SPRIT_X * PLAYER_EFFECT_SPRITE_SPRIT_Y)
    {
        cnt = 0;
    }
    DirectX::XMFLOAT4X4 world, view, proj;
    DirectX::XMMATRIX mView =
        DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),
            DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
            DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
        );
    DirectX::XMMATRIX mProj = DirectX::XMMatrixOrthographicOffCenterLH(
        0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0.0f, CMETER(30.0f), METER(2.0f));
    DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixTranspose(mView));
    DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixTranspose(mProj));
    Sprite::SetView(view);
    Sprite::SetProjection(proj);
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(PLAYER_EFFECT_SPRITE_POS_X, PLAYER_EFFECT_SPRITE_POS_Y, 0.0f);
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1.0f, -1.0f, 1.0f);// Y上下反転
    DirectX::XMMATRIX mWorld = S * T;// 拡縮→回転→移動
    mWorld = DirectX::XMMatrixTranspose(mWorld);// 転置
    DirectX::XMStoreFloat4x4(&world, mWorld);
    Sprite::SetWorld(world);
    Sprite::SetSize({ PLAYER_EFFECT_SPRITE_SIZE_X, PLAYER_EFFECT_SPRITE_SIZE_Y }); //　サイズ
    Sprite::SetOffset({ 0.0f, 0.0f });// 座標
    Sprite::SetUVPos({ 1.0f / PLAYER_EFFECT_SPRITE_SPRIT_X * (cnt % static_cast<int>(PLAYER_EFFECT_SPRITE_SPRIT_X)),1.0f / PLAYER_EFFECT_SPRITE_SPRIT_Y * (cnt / static_cast<int>(PLAYER_EFFECT_SPRITE_SPRIT_X)) });
    Sprite::SetUVScale({ 1.0f / PLAYER_EFFECT_SPRITE_SPRIT_X,1.0f / PLAYER_EFFECT_SPRITE_SPRIT_Y });

    Sprite::SetTexture(m_pEffectTexture);
	//エフェクトの色を半透明の灰色にする
    Sprite::SetColor({ 0.5f, 0.5f , 0.5f, 0.1f });
    Sprite::Draw();

}
//壁との当たり判定
void Player::OnCollisionWall()
{

    for (auto& itr : m_pWall)
    {
        Collision::Result result = Collision::Hit(GetCollision(), itr->GetCollision());
        if (result.isHit)
        {
            switch (itr->GetNo())
            {
            case WALL_FRONT:
                m_pos.z = itr->GetCollision().center.z + (GetCollision().size.z + itr->GetCollision().size.z + COLLISION_CORRECT) * 0.5f * result.normal.z;
                break;
            case WALL_UP:
                m_pos.y = itr->GetCollision().center.y + (GetCollision().size.y + itr->GetCollision().size.y + COLLISION_CORRECT) * 0.5f * result.normal.y;
                break;
            case WALL_LEFT:
                m_pos.x = itr->GetCollision().center.x + (GetCollision().size.x + itr->GetCollision().size.x + COLLISION_CORRECT) * 0.5f * result.normal.x;
                break;
            case WALL_RIGHT:
                m_pos.x = itr->GetCollision().center.x + (GetCollision().size.x + itr->GetCollision().size.x + COLLISION_CORRECT) * 0.5f* result.normal.x;
                break;
            case WALL_BACK:
                m_pos.z = itr->GetCollision().center.z + (GetCollision().size.z + itr->GetCollision().size.z + COLLISION_CORRECT) * 0.5f * result.normal.z;
                break;
            case WALL_DOWN:
                m_pos.y = itr->GetCollision().center.y + (GetCollision().size.y + itr->GetCollision().size.y + COLLISION_CORRECT) * 0.5f * result.normal.y;
                break;
            default:
                break;
            }
        }
    }
}
