#pragma once

#include "vortex_model.h"

//std includes
#include <memory>

namespace VortexEngine {

	struct Transform2DComponent {
		glm::vec2 translation{};
		glm::vec2 scale{1.0f, 1.0f};
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} };
			return rotMatrix * scaleMat;
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
		Transform2DComponent transform2D{};

	private:
		VortexGameObject(id_t objId) : id{ objId } {};

		id_t id;
	};
}