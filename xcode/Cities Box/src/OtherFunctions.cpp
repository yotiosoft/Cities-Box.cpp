//
//  OtherFunctions.cpp
//

#include "OtherFunctions.hpp"

vector<string> split(string str, string separator) {
	auto separator_length = separator.length();
	
	vector<string> list = vector<string>();
	
	if (separator_length == 0) {
		list.push_back(str);
	} else {
		auto offset = std::string::size_type(0);
		while (1) {
			auto pos = str.find(separator, offset);
			if (pos == std::string::npos) {
				list.push_back(str.substr(offset));
				break;
			}
			list.push_back(str.substr(offset, pos - offset));
			offset = pos + separator_length;
		}
	}
	
	return list;
}
