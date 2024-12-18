#include "../headers/vortex_app.h"
#include "../headers/render_system.h"
#include "../headers/vortex_camera.h"
#include "../headers/keyboard_movement_controller.h"
#include "../headers/mouse_movement_controller.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>

namespace VortexEngine {

	VortexApp::VortexApp() {
		loadGameObjects();
	}

	VortexApp::~VortexApp() {}

	void VortexApp::run() {
		RenderSystem renderSystem{ vortexDevice, vortexRenderer.getSwapChainRenderPass() };
        VortexCamera camera{};
        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 1.5f));

        auto viewerObject = VortexGameObject::createGameObject();
        KeyboardMovementController cameraController{};
		MouseMovementController mouseController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!vortexWindow.shouldClose()) {
			glfwPollEvents();

			glfwSetInputMode(vortexWindow.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(vortexWindow.getGLFWwindow(), frameTime, viewerObject);
			mouseController.lookAroundInPlaneXZ(vortexWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = vortexRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(70.0f), aspect, 0.1f, 10.0f);

			if (auto commandBuffer = vortexRenderer.beginFrame()) {

				//add more passes like shadows

				vortexRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				vortexRenderer.endSwapChainRenderPass(commandBuffer);
				vortexRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(vortexDevice.device());
	}

	void VortexApp::loadGameObjects() {
		std::shared_ptr<VortexModel> vortexModel = VortexModel::createModelFromFile(vortexDevice, "C:/Users/judet/OneDrive/Desktop/vortex_engine_proj_1/Assets/Meshes/smooth_teapot.obj");

        auto cube = VortexGameObject::createGameObject();
        cube.model = vortexModel;
        cube.transform.translation = { 0.0f, 0.0f, 1.5f };
        cube.transform.scale = { 0.5f, 0.5f, 0.5f };
        gameObjects.push_back(std::move(cube));
	}
}