#pragma once

#include "vortex_game_object.h"
#include "vortex_window.h"

namespace VortexEngine {
	class MouseMovementController {
	public:
		float sensitivity{ 1.2f };
		void lookAroundInPlaneXZ(GLFWwindow* window, float dt, VortexGameObject& gameObject);

	private:
		float rotX{};
		float rotY{};
	};
}