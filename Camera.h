#ifndef CAMERA_H
#define CAMERA_H

#include "Program.h"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 105.0f;


// static camera class that processes input for use in OpenGL

class StaticCamera
{
public:
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;
	float zoom;
	// forward angles
	float yaw;
	float pitch;

	// Dimensions
	int width;
	int height;

	glm::mat4 cameraMatrix{ 1.0f };

	// constructor with vectors
	StaticCamera(
		int p_width,
		int p_height,
		glm::vec3 p_position = glm::vec3(0.0f, 0.0f, 0.0f),
		float p_pitch = PITCH,
		float p_yaw = YAW,
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)
	) :
		forward(glm::vec3(0.0f, 0.0f, -1.0f)),
		width(p_width),
		height(p_height),
		zoom(ZOOM)
	{
		position = p_position;
		world_up = up;
		yaw = p_yaw;
		pitch = p_pitch;
		updateCameraVectors();
	}

	void updateMatrix(float FOVdeg, float nearPlane, float farPlane)
	{

		// Adds perspective to the scene

		// Sets new camera matrix
		cameraMatrix = glm::perspective(
			glm::radians(zoom),
			(float)width / height, nearPlane,
			farPlane) * glm::lookAt(
				position,
				position + forward,
				up);
	}

	void Matrix(Program& program, const char* uniform)
	{
		// Exports camera matrix
		glUniformMatrix4fv(glGetUniformLocation(program.getId(), uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
	}


protected:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front{ 0 };
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward = glm::normalize(front);
		// also re-calculate the Right and Up vector
		right = glm::normalize(glm::cross(forward, world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		up = glm::normalize(glm::cross(right, forward));
	}
};


class ControllableCamera : public StaticCamera
{
public:
	// camera movement
	float speed;
	float sensitivity;



	// constructor with vectors


	ControllableCamera(
		int width,
		int height,
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		float pitch = PITCH,
		float yaw = YAW,
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)
	) :
		StaticCamera(width, height, position, pitch, yaw, up),
		speed(SPEED),
		sensitivity(SENSITIVITY)
	{
		updateCameraVectors();
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void process_keyboard(Movement direction, float dt)
	{
		float velocity = speed * dt;
		if (direction == FORWARD)
			position += forward * velocity;
		if (direction == BACKWARD)
			position -= forward * velocity;
		if (direction == LEFT)
			position -= right * velocity;
		if (direction == RIGHT)
			position += right * velocity;
		if (direction == UP)
			position += world_up * velocity;
		if (direction == DOWN)
			position -= world_up * velocity;
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		zoom -= (float)yoffset;
		if (zoom < 1.0f)
			zoom = 1.0f;
		if (zoom > 105.0f)
			zoom = 105.0f;
	}
};
#endif