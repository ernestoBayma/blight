#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef float f32;
namespace Blight {
	enum class CameraDirection {
		Forward,
		Backward,
		Left,
		Right
	};

	constexpr f32 YAW 	  = -90.0f;
	constexpr f32 PITCH 	  = 0.0f;
	constexpr f32 SPEED 	  = 2.5f;
	constexpr f32 SENSITIVITY = 0.1f;
	constexpr f32 ZOOM	  = 45.0f;

	class Camera {
	public:
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;

		f32 Yaw;
		f32 Pitch;

		f32 MovementSpeed;
		f32 MouseSensitivity;
		f32 Zoom;

		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), f32 yaw = YAW, f32 pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
		{
			Position = position;
			WorldUp  = up;
			Yaw	 = yaw;
			Pitch    = pitch;

			updateCameraVectors();
		}

		Camera(f32 posX, f32 posY, f32 posZ, f32 upX, f32 upY, f32 upZ, f32 yaw, f32 pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
		{
			Position = glm::vec3(posX, posY, posZ);
			WorldUp  = glm::vec3(upX, upY, upZ);
			Yaw      = yaw;
			Pitch    = pitch;

			updateCameraVectors();
		}

		glm::mat4 GetViewMatrix();
		void ProcessKeyboard(CameraDirection direction, f32 deltaTime);
		void ProcessMouseMovement(f32 xoffset, f32 yoffset, bool constrainPitch = true);
		void ProcessMouseScroll(f32 yoffset);
	private:
		void updateCameraVectors() {
			glm::vec3 front;
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

			Front = glm::normalize(front);

			Right = glm::normalize(glm::cross(Front, WorldUp));
			Up    = glm::normalize(glm::cross(Right, Front));
		}
	};

};
