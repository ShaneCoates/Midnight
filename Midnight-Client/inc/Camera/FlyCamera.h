/*
	Author: Shane Coates
	Description: FlyCamera class
				 Inherits from Camera base class
*/

#ifndef _FLY_CAMERA_H_
#define	_FLY_CAMERA_H_

#include "Camera/Camera.h"
struct GLFWwindow;
class FlyCamera : public Camera
{
public:
	FlyCamera() : Camera()
	{
		Init();
	}
	FlyCamera(float _flySpeed) : Camera(), m_flySpeed(_flySpeed)
	{
		Init();
	}
	~FlyCamera() {};

	void Init();

	virtual void Update(double _dt);

	void SetFlySpeed(float _flySpeed)		{ m_flySpeed = _flySpeed; }
	float GetFlySpeed()						{ return m_flySpeed; }

	void SetRotationSpeed(float _rotationSpeed) { m_rotationSpeed = _rotationSpeed; }
	float GetRotationSpeed() { return m_rotationSpeed; }

	void SetInputWindow(GLFWwindow* _window)	{ m_window = _window; }
protected:
	void HandleKeyboardInput(double _dt);
	void HandleMouseInput(double _dt);

	void CalculateRotation(double dt, double xOffset, double yOffset);

	GLFWwindow* m_window;
	float m_flySpeed;
	float m_rotationSpeed = 1.0f; 

	bool m_bViewButtonClicked;
	double m_dCursorX, m_dCursorY;
};

#endif