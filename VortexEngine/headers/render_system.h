#pragma once

#include "../headers/vortex_pipeline.h"
#include "../headers/vortex_device.h"
#include "../headers/vortex_game_object.h"

#include <memory>
#include <vector>

namespace VortexEngine {
	class RenderSystem {

	public:

		RenderSystem(VortexDevice& device, VkRenderPass renderPass);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<VortexGameObject>& gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		VortexDevice& vortexDevice;

		std::unique_ptr<VortexPipeline> vortexPipeline;
		VkPipelineLayout pipelineLayout;
	};
}