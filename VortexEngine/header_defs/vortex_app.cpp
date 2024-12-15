#include "../headers/vortex_app.h"

#include <stdexcept>
#include <array>

namespace VortexEngine {

	VortexApp::VortexApp() {
		loadModels();
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

	void VortexApp::loadModels() {
		std::vector<VortexModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}
		};

		vortexModel = std::make_unique<VortexModel>(vortexDevice, vertices);
	}

	void VortexApp::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
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
		clearValues[0].color = { 0.1f, 0.1f, 0.1f };
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

		vortexPipeline->bind(commandBuffers[imageIndex]);
		vortexModel->bind(commandBuffers[imageIndex]);
		vortexModel->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
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