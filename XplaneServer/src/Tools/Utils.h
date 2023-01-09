#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string ExtractValueJson(json elem);