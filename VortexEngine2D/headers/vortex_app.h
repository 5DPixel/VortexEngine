#pragma once

#include "../headers/vortex_window.h"
#include "../headers/vortex_device.h"
#include "../headers/vortex_game_object.h"
#include "../headers/vortex_renderer.h"

#include <memory>
#include <vector>

namespace VortexEngine {
	class VortexApp {

	public:
		static constexpr int width = 800;
		static constexpr int height = 600;

		VortexApp();
		~VortexApp();

		VortexApp(const VortexApp&) = delete;
		VortexApp& operator=(const VortexApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		VortexWindow vortexWindow{ width, height, "Vortex Engine" };
		VortexDevice vortexDevice{ vortexWindow };
		VortexRenderer vortexRenderer{ vortexWindow, vortexDevice };

		std::vector<VortexGameObject> gameObjects;
	};
}