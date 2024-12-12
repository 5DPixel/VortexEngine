#pragma once

#include "../headers/vortex_window.h"
#include "../headers/vortex_pipeline.h"
#include "../headers/vortex_device.h"
#include "../headers/vortex_swap_chain.h"

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
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		VortexWindow vortexWindow{ width, height, "Testing Window" };
		VortexDevice vortexDevice{ vortexWindow };
		VortexSwapChain vortexSwapChain{ vortexDevice, vortexWindow.getExtent() };
		std::unique_ptr<VortexPipeline> vortexPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}