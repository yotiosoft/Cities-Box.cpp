#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>

#include "Header.hpp"
#include "OtherFunctions.hpp"

using namespace std;

namespace specific {
	vector<FileStruct> getAllFilesName(string folderPath, string extension);
}
