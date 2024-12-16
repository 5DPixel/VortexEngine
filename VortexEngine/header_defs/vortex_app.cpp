#include "../headers/vortex_app.h"
#include "../headers/render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace VortexEngine {

	VortexApp::VortexApp() {
		loadGameObjects();
	}

	VortexApp::~VortexApp() {}

	void VortexApp::run() {
		RenderSystem renderSystem{ vortexDevice, vortexRenderer.getSwapChainRenderPass() };

		while (!vortexWindow.shouldClose()) {
			glfwPollEvents();

			if (auto commandBuffer = vortexRenderer.beginFrame()) {

				//add more passes like shadows

				vortexRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(commandBuffer, gameObjects);
				vortexRenderer.endSwapChainRenderPass(commandBuffer);
				vortexRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(vortexDevice.device());
	}

	void VortexApp::loadGameObjects() {
		std::vector<VortexModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}
		};

		auto vortexModel = std::make_shared<VortexModel>(vortexDevice, vertices);

		auto triangle = VortexGameObject::createGameObject();
		triangle.model = vortexModel;
		triangle.color = { 0.1f, 0.8f, 0.1f };
		triangle.transform2D.translation.x = 0.2f;
		triangle.transform2D.scale = { 2.0f, 0.5f };
		triangle.transform2D.rotation = 0.25f * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
	}
}