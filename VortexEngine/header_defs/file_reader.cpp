#include "../headers/file_reader.h"

namespace VortexEngine {
	void FileReader::init() {
		fileStream = std::ifstream{ filepath };
	}

	FileReader::FileReader(std::string filePath){
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