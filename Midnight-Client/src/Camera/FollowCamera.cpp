#include "Camera\FollowCamera.h"

void FollowCamera::SetOffsetFromTarget(glm::vec3 _offset)
{
	m_offsetFromTarget = _offset;
}

void FollowCamera::Update(double dt)
{

}

void FollowCamera::UpdatePosition(glm::vec3 _target, glm::vec3 _offset)
{
	SetLookAt(_target + m_offsetFromTarget, _target + _offset, glm::vec3(0, 1, 0));
}
