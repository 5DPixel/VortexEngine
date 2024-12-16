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


		glm::mat4 mat4() {
			auto transform = glm::translate(glm::mat4{ 1.0f }, translation);

			transform = glm::rotate(transform, rotation.y, {0.0f, 1.0f, 0.0f});
			transform = glm::rotate(transform, rotation.x, { 1.0f, 0.0f, 0.0f });
			transform = glm::rotate(transform, rotation.z, { 0.0f, 0.0f, 1.0f });

			transform = glm::scale(transform, scale);

			return transform;
		}
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