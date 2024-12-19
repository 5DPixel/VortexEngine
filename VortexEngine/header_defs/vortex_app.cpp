#include "../headers/vortex_app.h"

#include "../headers/render_system.h"
#include "../headers/vortex_camera.h"
#include "../headers/keyboard_movement_controller.h"
#include "../headers/mouse_movement_controller.h"
#include "../headers/vortex_buffer.h"
#include "../headers/json.h"
#include "../headers/file_reader.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>

#include <stdexcept>
#include <array>
#include <chrono>
#include <string>

namespace VortexEngine {

	struct GlobalUbo {
		glm::mat4 projectionView{ 1.0f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.0f, -3.0f, -1.0f });
	};

	VortexApp::VortexApp() {
		globalPool = VortexDescriptorPool::Builder(vortexDevice)
			.setMaxSets(VortexSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VortexSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadGameObjects();
	}

	VortexApp::~VortexApp() {}

	void VortexApp::run() {

		std::vector<std::unique_ptr<VortexBuffer>> uboBuffers(VortexSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<VortexBuffer>(
				vortexDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);

			uboBuffers[i]->map();
		}

			auto globalSetLayout = VortexDescriptorSetLayout::Builder(vortexDevice)
				.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build();

			std::vector<VkDescriptorSet> globalDescriptorSets(VortexSwapChain::MAX_FRAMES_IN_FLIGHT);

			for (int i = 0; i < globalDescriptorSets.size(); i++) {
				auto bufferInfo = uboBuffers[i]->descriptorInfo();
				VortexDescriptorWriter(*globalSetLayout, *globalPool)
					.writeBuffer(0, &bufferInfo)
					.build(globalDescriptorSets[i]);
			}

		VortexBuffer globalUboBuffer{
			vortexDevice,
			sizeof(GlobalUbo),
			VortexSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			vortexDevice.properties.limits.minUniformBufferOffsetAlignment,
		};

		globalUboBuffer.map();


		RenderSystem renderSystem{ vortexDevice, vortexRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        VortexCamera camera{};
        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 1.5f));

        auto viewerObject = VortexGameObject::createGameObject();
        KeyboardMovementController cameraController{};
		MouseMovementController mouseController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!vortexWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(vortexWindow.getGLFWwindow(), frameTime, viewerObject);
			mouseController.lookAroundInPlaneXZ(vortexWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = vortexRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(70.0f), aspect, 0.1f, 10.0f);

			if (auto commandBuffer = vortexRenderer.beginFrame()) {
				int frameIndex = vortexRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex]
				};

				//update

				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//render pass
				vortexRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(frameInfo, gameObjects);
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

		FileReader main{ "C:/Users/judet/OneDrive/Desktop/vortex_engine_proj_1/Assets/Scenes/main.vscn" };
		nlohmann::json sceneJson = nlohmann::json::parse(main.read());
		std::cout << sceneJson["gameObjects"];
	}
}