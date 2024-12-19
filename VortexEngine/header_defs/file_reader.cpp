#include "../headers/file_reader.h"

namespace VortexEngine {
	void FileReader::init() {
		fileStream = std::ifstream{ filepath };
	}

	FileReader::FileReader(std::string filePath){
		if (!std::filesystem::exists(filePath)) {
			std::string errorMessage = "Error: Scene file " + filepath + " did not exist!";
			assert(false && errorMessage.c_str());
		}

		filepath = filePath;
		init();
	}

	std::string FileReader::read() {
		std::string returnContents{};
		while (getline(fileStream, fileContents)) {
			returnContents += fileContents;
		}

		return returnContents;
	}
}