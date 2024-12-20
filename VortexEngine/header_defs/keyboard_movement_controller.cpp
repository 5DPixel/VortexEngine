#include "../headers/keyboard_movement_controller.h"
#include <iostream>

namespace VortexEngine {
	void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, VortexGameObject& gameObject) {
		glm::vec3 rotate{ 0 };
		
		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) {
			rotate.y++;
		}

		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) {
			rotate.y--;
		}

		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) {
			rotate.x++;
		}

		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) {
			rotate.x--;
		}

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
		}

		gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
		gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

		float yaw = gameObject.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.0f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
		const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

		glm::vec3 moveDir{ 0.0f };

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
					if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) {
			moveDir += forwardDir;
		}

		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) {
			moveDir -= forwardDir;
		}

		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) {
			moveDir += rightDir;
		}

		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) {
			moveDir -= rightDir;
		}

		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) {
			moveDir += upDir;
		}

		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) {
			moveDir -= upDir;
		}
		}

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			gameObject.transform.translation += lookSpeed * dt * glm::normalize(moveDir);
		}
	}
}