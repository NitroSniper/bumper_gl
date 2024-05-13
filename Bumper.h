#ifndef BUMPER_H
#define BUMPER_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Camera.h" // just for keyboard controls
#include "AABB.h"

class Bumper {
private:
	glm::mat4 model{ 1 };
public:
	glm::vec3 position;
	glm::float32 velocity{ 0 };
	glm::float32 handle{ 45 };
	glm::float32 braking{ 15 };
	glm::float32 power{ 9 };
	glm::float32 yaw{ 90 };
	glm::float32 pitch{ 0 };
	glm::float32 scale;
	glm::float32 friction{ 2 };
	glm::vec3 forward{ 0 };
	AABB box{0.5, 0.9, 1};

	std::vector<AABB*> collisions{};

	Bumper(glm::vec3 pos, glm::float32 scl) : position(pos), scale(scl) {

	}

	void update(float dt) {


		// update forward vec
		forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward.y = sin(glm::radians(pitch));
		forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward = glm::normalize(forward) * velocity;


		velocity = glm::sign(velocity) * glm::max(glm::abs(velocity) - friction * dt, 0.0f);

		position += forward * dt;


		box.update_from_pos_and_dim(position);
		for (AABB* other_box : collisions) {
			if (box.intersects(*other_box)) {
				while (box.intersects(*other_box)) {
					position -= forward * dt;
					box.update_from_pos_and_dim(position);
				}
				velocity /= -2;
			}
		}

		// update model (dynamic)
		model = glm::translate(glm::mat4(1), position); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(scale));	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, glm::radians(90 - yaw), glm::vec3(0, 1, 0));
	}

	void draw(Program& program, Model& modal) {
		glUniformMatrix4fv(glGetUniformLocation(program.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		modal.Draw(program);
	}

	void process_keyboard(Movement direction, float dt)
	{
		if (direction == UP)
			velocity += power * dt;
		if (direction == DOWN)
			velocity -= braking * dt;
		if (direction == FORWARD)
			velocity += power * dt;
		if (direction == BACKWARD)
			velocity -= braking * dt;
		if (direction == LEFT)
			yaw -= handle * dt;
		if (direction == RIGHT)
			yaw += handle * dt;
	}
};
#endif
