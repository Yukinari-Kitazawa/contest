#include "TitlePlayer.h"

//==============
//ƒ^ƒCƒgƒ‹‚Ì‰_‚Í“®‚«‚¾‚¯’S“–
//Œp³Œã‚Ì‚±‚±‚Å‚Í“–‚½‚Á‚½Žž‚Ìˆ—‚ð‘‚­
//==============

TitlePlayer::TitlePlayer(DirectX::XMFLOAT3 InitPos) : TitleTrashObject(InitPos)
{
	
	m_pModel = ModelCache::GetInstance()->GetCache("Player");
}

TitlePlayer::~TitlePlayer()
{
}

void TitlePlayer::Update()
{
	TitleTrashObject::Update();

}

void TitlePlayer::Draw()
{
	TitleTrashObject::Draw();
}
