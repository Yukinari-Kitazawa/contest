#pragma once
#include "TitleTrashObject.h"
class TitlePlayer : public TitleTrashObject
{
public:
	TitlePlayer(DirectX::XMFLOAT3 InitPos);
	~TitlePlayer() final;
	void Update() final;
	void Draw() final;
};

