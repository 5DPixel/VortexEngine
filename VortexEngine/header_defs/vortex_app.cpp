#include "../headers/vortex_app.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace VortexEngine {

	struct SimplePushConstantData {
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	VortexApp::VortexApp() {
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	VortexApp::~VortexApp() {
		vkDestroyPipelineLayout(vortexDevice.device(), pipelineLayout, nullptr);
	}

	void VortexApp::run() {
		while (!vortexWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
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

	void VortexApp::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(vortexDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void VortexApp::createPipeline() {
		assert(vortexSwapChain != nullptr && "Cannot create pipeline before swap chain!");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		VortexPipeline::defaultPipelineConfigInfo(
			pipelineConfig
		);

		pipelineConfig.renderPass = vortexSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vortexPipeline = std::make_unique<VortexPipeline>(
			vortexDevice,
			"shaders/default.vert.spv",
			"shaders/default.frag.spv",
			pipelineConfig
		);
	}

	void VortexApp::recreateSwapChain() {
		auto extent = vortexWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = vortexWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(vortexDevice.device());
		
		if (vortexSwapChain == nullptr) {
			vortexSwapChain = std::make_unique<VortexSwapChain>(vortexDevice, extent);
		}
		else {
			vortexSwapChain = std::make_unique<VortexSwapChain>(vortexDevice, extent, std::move(vortexSwapChain));
			if (vortexSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		createPipeline();
	}

	void VortexApp::createCommandBuffers() {
		commandBuffers.resize(vortexSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vortexDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(vortexDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers!");
		}
	}

	void VortexApp::freeCommandBuffers() {
		vkFreeCommandBuffers(
			vortexDevice.device(),
			vortexDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data()
		);

		commandBuffers.clear();
	}

	void VortexApp::recordCommandBuffer(int imageIndex) {

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vortexSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vortexSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vortexSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2>clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(vortexSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(vortexSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0.0f, 0.0f}, vortexSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		renderGameObjects(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}
	}

	void VortexApp::renderGameObjects(VkCommandBuffer commandBuffer) {
		vortexPipeline->bind(commandBuffer);

		for (auto& obj : gameObjects) {
			obj.transform2D.rotation = glm::mod(obj.transform2D.rotation + 0.01f, glm::two_pi<float>());

			SimplePushConstantData push{};

			push.offset = obj.transform2D.translation;
			push.color = obj.color;
			push.transform = obj.transform2D.mat2();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);

			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}

	void VortexApp::drawFrame() {
		uint32_t imageIndex;
		auto result = vortexSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		recordCommandBuffer(imageIndex);

		result = vortexSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vortexWindow.wasWindowResized()) {
			vortexWindow.resetWindowResizedFlag();
			recreateSwapChain();

			return;
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present chain image!");
		}
	}
}