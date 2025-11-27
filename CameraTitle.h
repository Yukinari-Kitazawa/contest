#pragma once
#include "Camera.h"
class CameraTitle :public Camera
{
public:
	CameraTitle();
	~CameraTitle();
	void Update()final;
private:
	float m_radXZ;
	float m_radY;
	float m_radius;
};

