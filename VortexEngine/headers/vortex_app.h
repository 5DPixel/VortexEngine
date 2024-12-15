#pragma once

#include "../headers/vortex_window.h"
#include "../headers/vortex_pipeline.h"
#include "../headers/vortex_device.h"
#include "../headers/vortex_game_object.h"
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
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		VortexWindow vortexWindow{ width, height, "Vortex Engine" };
		VortexDevice vortexDevice{ vortexWindow };
		std::unique_ptr<VortexSwapChain> vortexSwapChain;
		std::unique_ptr<VortexPipeline> vortexPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VortexGameObject> gameObjects;
	};
}