#pragma once

#include <string>
#include <fstream>
#include <iostream>

namespace VortexEngine {
	class FileReader {
	public:
		FileReader(std::string filePath);
		std::string read();

	private:
		std::ifstream fileStream{};
		std::string fileContents{};
		std::string filepath{};
		void init();
	};
}