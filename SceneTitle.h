#pragma once
#include "Scene.h"
#include "Texture.h"
#include"Sprite.h"
#include"Sound.h"
#include"Camera.h"
#include"SkyDome.h"
#include"TitleTrashObject.h"
#include"TitlePlayer.h"
#include"vector"
#include"Particle.h"
#include<memory>

class SceneTitle :public Scene
{
public:
	SceneTitle();
	~SceneTitle();

	// 更新処理 
	void Update() final;

	// 描画処理 
	void Draw() final;

private:
	Texture* m_pTitle; // タイトル画面に表示する画像 
	Texture* m_pLogo; // タイトル画面に表示する画像 
	Texture* m_pEnter; // タイトル画面に表示する画像 
	IXAudio2SourceVoice* m_pspeaker;
	IXAudio2SourceVoice* m_pspeaker2;
	IXAudio2SourceVoice* m_pspeaker3;
	XAUDIO2_BUFFER* m_pBgm;
	XAUDIO2_BUFFER* m_pSe;
	XAUDIO2_BUFFER* m_pCollisionSe;
	Camera* m_pTitleCamera;
	SkyDome* m_pSkyDome;
	std::vector<TitleTrashObject*> m_TitleTrashObjectList;
	std::list<DirectX::XMFLOAT3> m_EffectPos;
	TitlePlayer* m_pTitlePlayer;
	std::vector<std::shared_ptr<Particle>>m_pParticle;
	bool m_wasColliding = false;

};

