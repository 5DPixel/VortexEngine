#pragma once

#include "vortex_camera.h"
#include <vulkan/vulkan.h>

namespace VortexEngine {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		VortexCamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}