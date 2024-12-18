#include "../headers/mouse_movement_controller.h"

namespace VortexEngine {
	void MouseMovementController::lookAroundInPlaneXZ(GLFWwindow* window, float dt, VortexGameObject& gameObject) {
		static double lastX{ 0 }, lastY{ 0 };
		double xPos{}, yPos{};

		glm::vec3 rotate{ 0 };

		glfwGetCursorPos(window, &xPos, &yPos);

		double dx = xPos - lastX;
		double dy = yPos - lastY;

		lastX = xPos;
		lastY = yPos;

		rotate.y = sensitivity * static_cast<float>(dx);
		rotate.x = -sensitivity * static_cast<float>(dy);

		rotate.x = glm::clamp(rotate.x, -glm::radians(89.0f), glm::radians(89.0f));

		gameObject.transform.rotation += rotate;
	}
}