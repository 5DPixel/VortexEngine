#pragma once

#include "vortex_pipeline.h"
#include "vortex_device.h"
#include "vortex_game_object.h"
#include "vortex_camera.h"
#include "vortex_frame_info.h"

#include <memory>
#include <vector>

namespace VortexEngine {
	class RenderSystem {

	public:

		RenderSystem(VortexDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		void renderGameObjects(FrameInfo &frameInfo, std::vector<VortexGameObject>& gameObjects);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VortexDevice& vortexDevice;

		std::unique_ptr<VortexPipeline> vortexPipeline;
		VkPipelineLayout pipelineLayout;
	};
}