#pragma once

#include "../headers/vortex_window.h"
#include "../headers/vortex_device.h"
#include "../headers/vortex_swap_chain.h"

#include <memory>
#include <vector>
#include <cassert>

namespace VortexEngine {
	class VortexRenderer {

	public:

		VortexRenderer(VortexWindow &window, VortexDevice &device);
		~VortexRenderer();

		VortexRenderer(const VortexRenderer&) = delete;
		VortexRenderer& operator=(const VortexRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const {
			return vortexSwapChain->getRenderPass();
		}
		float getAspectRatio() const {
			return vortexSwapChain->extentAspectRatio();
		}

		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress!");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress!");

			return currentFrameIndex;
		}


		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		VortexWindow& vortexWindow;
		VortexDevice& vortexDevice;
		std::unique_ptr<VortexSwapChain> vortexSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
	};
}