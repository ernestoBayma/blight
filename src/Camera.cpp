#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Camera.h>

using namespace Blight;

glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(CameraDirection direction, float deltaTime) {
	float velocity = MovementSpeed * deltaTime;
	if(direction == CameraDirection::Forward) 
		Position += Front * velocity;
	if(direction == CameraDirection::Backward)
		Position -= Front * velocity;
	if(direction == CameraDirection::Left) 
		Position -= Right * velocity;
	if(direction == CameraDirection::Right)
		Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw 	+= xoffset;
	Pitch 	+= yoffset;

	if(constrainPitch) {
		if(Pitch > 89.0f) 
			Pitch = 89.0f;
		if(Pitch < -89.0f)
			Pitch -= 89.0f;
	}

	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	Zoom -= static_cast<float>(yoffset);
	if(Zoom < 1.0f)
		Zoom = 1.0f;
	if(Zoom > 90.0f)
		Zoom = 90.0f;
}
