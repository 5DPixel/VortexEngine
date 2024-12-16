#include "../headers/vortex_renderer.h"

#include <cassert>
#include <stdexcept>
#include <array>

namespace VortexEngine {

	VortexRenderer::VortexRenderer(VortexWindow& window, VortexDevice& device) : vortexWindow{ window }, vortexDevice{device} {
		recreateSwapChain();
		createCommandBuffers();
	}

	VortexRenderer::~VortexRenderer() {
		freeCommandBuffers();
	}

	void VortexRenderer::recreateSwapChain() {
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
			std::shared_ptr<VortexSwapChain> oldSwapChain = std::move(vortexSwapChain);
			vortexSwapChain = std::make_unique<VortexSwapChain>(vortexDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*vortexSwapChain.get())) {
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}

		//can't create pipeline
	}

	void VortexRenderer::createCommandBuffers() {
		commandBuffers.resize(VortexSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vortexDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(vortexDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers!");
		}
	}

	void VortexRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(
			vortexDevice.device(),
			vortexDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data()
		);

		commandBuffers.clear();
	}

	VkCommandBuffer VortexRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call begin frame whilst already in progress!");

		auto result = vortexSwapChain->acquireNextImage(&currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void VortexRenderer::endFrame() {
		assert(isFrameInProgress() && "Can't call endFrame while frame is not in progress!");

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}

		auto result = vortexSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vortexWindow.wasWindowResized()) {
			vortexWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % VortexSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void VortexRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vortexSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vortexSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vortexSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2>clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(vortexSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(vortexSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0.0f, 0.0f}, vortexSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void VortexRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}
}