#include "../headers/mouse_movement_controller.h"

namespace VortexEngine {
	void MouseMovementController::lookAroundInPlaneXZ(GLFWwindow* window, float dt, VortexGameObject& gameObject) {

		static double lastX{ 0 }, lastY{ 0 };
		double xPos{}, yPos{};

		glm::vec3 rotate{ 0 };

		glfwGetCursorPos(window, &xPos, &yPos);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
			GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
			glfwSetCursor(window, cursor);

			double dx = xPos - lastX;
			double dy = yPos - lastY;

			rotate.y = sensitivity * static_cast<float>(dx) * dt;
			rotate.x = -sensitivity * static_cast<float>(dy) * dt;

			rotate.x = glm::clamp(rotate.x, -glm::radians(89.0f), glm::radians(89.0f));

			gameObject.transform.rotation += rotate;
		}
		else {
			GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
			glfwSetCursor(window, cursor);
		}

		lastX = xPos;
		lastY = yPos;
	}
}
