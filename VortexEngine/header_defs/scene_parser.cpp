#include "../headers/scene_parser.h"

#include <iostream>

namespace VortexEngine {
	void SceneParser::init() {
		fileStream = std::ifstream{ filepath };
		if (!fileStream.is_open()) {
			throw std::runtime_error("Error: Could not open scene file " + filepath);
		}
	}

	SceneParser::SceneParser(std::string filePath){
		if (!std::filesystem::exists(filePath)) {
			std::string errorMessage = "Error: Scene file " + filepath + " did not exist!";
			assert(false && errorMessage.c_str());
		}

		filepath = filePath;
		init();
		read();
	}

	void SceneParser::read() {
		assert(fileStream && "Cannot call function read of SceneParser class before init!");

		while (getline(fileStream, fileContents)) {
			fileReadContents += fileContents;
		}

		fileStream.close();
	}

	SceneParser::~SceneParser() {
		if (fileStream.is_open()) {
			fileStream.close();
		}
	}

	std::vector<VortexGameObject> SceneParser::parseScene(VortexDevice& vortexDevice) {
		parsedJsonData = nlohmann::json::parse(fileReadContents);
		std::vector<VortexGameObject> gameObjects;

		for (auto& obj : parsedJsonData["gameObjects"]) {
			std::string filePath = obj["file"];
			std::shared_ptr<VortexModel> objectModel = VortexModel::createModelFromFile(vortexDevice, filePath);

			glm::vec3 position = {
				obj["position"][0],
				obj["position"][1],
				obj["position"][2]
			};

			glm::vec3 scale = {
				obj["scale"][0],
				obj["scale"][1],
				obj["scale"][2]
			};

			glm::vec3 rotation = {
				obj["rotation"][0],
				obj["rotation"][1],
				obj["rotation"][2]
			};

			auto object = VortexGameObject::createGameObject();
			object.model = objectModel;
			object.transform.translation = position;
			object.transform.scale = scale;
			gameObjects.push_back(std::move(object));
		}

		return gameObjects;
	}
}