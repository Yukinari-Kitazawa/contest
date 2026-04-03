#pragma once
#include<vector>
#include"CameraPlayer.h"
#include"Sound.h"
class Player;
class TrashObject;
class Wall;
class Goal;
class Model;
class SkyDome;
class StageObjectManager
{
public:
	StageObjectManager();
	~StageObjectManager();
	void Update();
	void Draw();
	void SetPlayer(Player* Inplayer) { m_pPlayer = Inplayer; }
	std::vector <Wall*> GetWall(){ return m_WallObjects; }
	int GetStageObjectCnt() { return m_StageObjects.size(); }
	void SetCamera(CameraPlayer* InCamera);
private:
	std::vector<TrashObject*> m_StageObjects;
	std::vector <Wall*> m_WallObjects;
	Player* m_pPlayer;
	Goal* m_pGoal;
	CameraPlayer* m_pCamera;
	SkyDome* m_pSkyDome;
	IXAudio2SourceVoice* m_pSEspeaker;
	XAUDIO2_BUFFER* m_pGetSe;
};

