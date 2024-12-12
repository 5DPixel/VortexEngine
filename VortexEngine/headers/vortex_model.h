#pragma once

#include "vortex_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std includes
#include <vector>

namespace VortexEngine {
	class VortexModel {
	public:
		struct Vertex {
			glm::vec2 position;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		VortexModel(VortexDevice& vortexDevice, const std::vector<Vertex>& vertices);
		~VortexModel();

		VortexModel(const VortexModel&) = delete;
		VortexModel& operator=(const VortexModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);


	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);

		VortexDevice& vortexDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}