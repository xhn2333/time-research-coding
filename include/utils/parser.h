#ifndef UTILS_PARSER_H
#define UTILS_PARSER_H
#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static json parseJsonString(const std::string& jsonString) {
	try {
		json j = json::parse(jsonString);
		return j;
	} catch (json::parse_error& e) {
		std::cerr << "Parse error: " << e.what() << std::endl;
		return json();
	}
}

#endif	// UTILS_PARSER_H