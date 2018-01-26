#pragma once
#include "Engine/Math/Vector2.hpp"

enum XboxButtonID
{
	XBOX_DPAD_UP = 0x0001,
	XBOX_DPAD_DOWN = 0x0002,
	XBOX_DPAD_LEFT = 0x0004,
	XBOX_DPAD_RIGHT = 0x0008,
	XBOX_START = 0x0010,
	XBOX_BACK = 0x0020,
	XBOX_LEFT_THUMB = 0x0040,
	XBOX_RIGHT_THUMB = 0x0080,
	XBOX_LEFT_SHOULDER = 0x0100,
	XBOX_RIGHT_SHOULDER = 0x0200,
	XBOX_A = 0x1000,
	XBOX_B = 0x2000,
	XBOX_X = 0x4000,
	XBOX_Y = 0x8000,
	XBOX_RIGHT_TRIGGER = 0x5806,
	XBOX_LEFT_TRIGGER = 0x5807
};

struct XboxStick
{
	XboxStick() :
		m_position(Vector2::ZERO),
		m_magnitude(0.0f),
		m_degrees(0.0f)
	{}

	Vector2	m_position;
	float m_magnitude;
	float m_degrees;

	void UpdateFromRawPosition(short rawX, short rawY);
	float GetOrientationAs360Degrees();
};

class XboxController
{
public:
	XboxController();
	XboxController(bool connected, int num);
	~XboxController();
	void Update();
	void VibrateController(int controllerIndex, int leftMotorSpeed, int rightMotorSpeed);
public:
	bool		m_isConnected;
	XboxStick	m_leftStick;
	XboxStick	m_rightStick;
	int			m_controllerID;
	float		m_leftTrigger;
	float		m_rightTrigger;
	int			m_wButton;
	int			m_wButtonLastState;
};