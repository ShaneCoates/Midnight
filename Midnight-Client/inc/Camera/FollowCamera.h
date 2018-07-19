/*
Author: Shane Coates
Description: FlyCamera class
Inherits from Camera base class
*/

#ifndef _FOLLOW_CAMERA_H_
#define	_FOLLOW_CAMERA_H_

#include "Camera/Camera.h"
struct GLFWwindow;
class FollowCamera : public Camera
{
public:
	FollowCamera() : Camera() {};
	~FollowCamera() {};

	void SetOffsetFromTarget(glm::vec3 _offset);

	virtual void Update(double _dt);
	void UpdatePosition(glm::vec3 _target, glm::vec3 _offset);

protected:

	GLFWwindow* m_window;

	glm::vec3 m_offsetFromTarget;
};

#endif