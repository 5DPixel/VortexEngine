#pragma once

#include <string>
#include <fstream>
#include <cassert>
#include <filesystem>
#include <vector>

#include "vortex_game_object.h"
#include "json.h"

namespace VortexEngine {
	class SceneParser {
	public:
		SceneParser(std::string filePath);
		~SceneParser();
		std::vector<VortexGameObject> parseScene(VortexDevice& vortexDevice);

	private:
		std::ifstream fileStream{};
		std::string fileContents{};
		std::string fileReadContents{};
		std::string filepath{};
		nlohmann::json parsedJsonData{};
		std::vector<std::shared_ptr<VortexGameObject>> currentGameObjects{};
		void read();
		void init();
	};
}