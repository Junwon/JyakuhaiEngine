#include "Engine//Input/XboxController.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h> // include the Xinput API
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; using 9_1_0 explicitly


void XboxStick::UpdateFromRawPosition(short rawX, short rawY)
{
	float floatRawX = (float)rawX;
	float floatRawY = (float)rawY;
	float unitX = RangeMapFloat(floatRawX, -32768.f, 32767.f, -1.0f, 1.0f);
	float unitY = RangeMapFloat(floatRawY, -32768.f, 32767.f, -1.0f, 1.0f);

	float degrees = Atan2Degrees(unitY, unitX);
	float rawMagnitude = CalcDistance(Vector2::ZERO, Vector2(unitX, unitY));

	//XBOX_STICK_DEADZONE_MIN/MAX
	float correctedMagnitude = RangeMapFloat(rawMagnitude, 0.35f, 0.95f, 0.0f, 1.0f);
	correctedMagnitude = ClampFloat(correctedMagnitude, 0.f, 1.0f);
	
	m_magnitude = correctedMagnitude;
	if (m_magnitude > 0.f)
	{
		m_degrees = degrees;
	}
	m_position.SetLengthAndHeadingDegrees(m_magnitude, m_degrees);
}

float XboxStick::GetOrientationAs360Degrees()
{
	float returnDegrees = m_degrees;
	if (returnDegrees < 0.f) {
		returnDegrees = 360.f + returnDegrees;
	}

	return returnDegrees;
}

XboxController::XboxController() :
	m_isConnected(false),
	m_controllerID(-1),
	m_leftTrigger(0),
	m_rightTrigger(0),
	m_wButton(0),
	m_wButtonLastState(0)
{

}

XboxController::XboxController(bool connected, int num) :
	m_isConnected(connected),
	m_controllerID(num),
	m_leftTrigger(0),
	m_rightTrigger(0),
	m_wButton(0),
	m_wButtonLastState(0)
{
}

XboxController::~XboxController()
{

}

void XboxController::Update()
{
	m_wButtonLastState = m_wButton;
	XINPUT_STATE xboxControllerState;
	memset(&xboxControllerState, 0, sizeof(xboxControllerState));
	DWORD errorStatus = XInputGetState(m_controllerID, &xboxControllerState);
	if (errorStatus == ERROR_DEVICE_NOT_CONNECTED) {
		m_isConnected = false;
		return;
	}

	m_isConnected = true;

	short leftRawStickX = xboxControllerState.Gamepad.sThumbLX;
	short leftRawStickY = xboxControllerState.Gamepad.sThumbLY;
	short rightRawStickX = xboxControllerState.Gamepad.sThumbRX;
	short rightRawStickY = xboxControllerState.Gamepad.sThumbRY;

	m_leftStick.UpdateFromRawPosition(leftRawStickX, leftRawStickY);
	m_rightStick.UpdateFromRawPosition(rightRawStickX, rightRawStickY);

	m_wButton = xboxControllerState.Gamepad.wButtons;
	
	//Update trigger position
	unsigned char leftRawTrigger = xboxControllerState.Gamepad.bLeftTrigger;
	unsigned char rightRawTrigger = xboxControllerState.Gamepad.bRightTrigger;

	float floatLeftRawTrigger = (float)leftRawTrigger;
	float floatRightRawTrigger = (float)rightRawTrigger;
	m_leftTrigger = RangeMapFloat(floatLeftRawTrigger, 0.0f, 255.0f, 0.0f, 1.0f);
	m_rightTrigger = RangeMapFloat(floatRightRawTrigger, 0.0f, 255.0f, 0.0f, 1.0f);
}

void XboxController::VibrateController(int controllerIndex, int leftMotorSpeed, int rightMotorSpeed)
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = (WORD) leftMotorSpeed;		// use any value between 0-65535 here
	vibration.wRightMotorSpeed = (WORD) rightMotorSpeed;	// use any value between 0-65535 here
	XInputSetState(controllerIndex, &vibration);
}