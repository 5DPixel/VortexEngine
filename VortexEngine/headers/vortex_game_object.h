#pragma once

#include "vortex_model.h"

//libs
#include <glm/gtc/matrix_transform.hpp>
//std includes
#include <memory>

namespace VortexEngine {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{1.0f, 1.0f, 1.0f};
		glm::vec3 rotation{};

		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class VortexGameObject {
	public:
		using id_t = unsigned int;

		static VortexGameObject createGameObject() {
			static id_t currentId = 0;
			return VortexGameObject{ currentId++ };
		}

		VortexGameObject(const VortexGameObject&) = delete;
		VortexGameObject& operator=(const VortexGameObject&) = delete;
		VortexGameObject(VortexGameObject&&) = default;
		VortexGameObject& operator=(VortexGameObject&&) = default;

		id_t getId() { return id; }

		std::shared_ptr<VortexModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		VortexGameObject(id_t objId) : id{ objId } {};

		id_t id;
	};
}