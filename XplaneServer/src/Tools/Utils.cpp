#include "Utils.h"

std::string ExtractValueJson(json elem)
{
	std::string value = "";
	switch (elem.type())
	{
	case json::value_t::string:
		value = elem.get<std::string>();
		break;
	case json::value_t::number_float:
		value = std::to_string(elem.get<float>());
		break;
	case json::value_t::number_integer:
		value = std::to_string(elem.get<int>());
		break;
	case json::value_t::number_unsigned:
		value = std::to_string(elem.get<unsigned int>());
		break;
	case json::value_t::object:
		value = elem.dump();
		break;
	case json::value_t::array:
		value = elem.dump();
		break;
	default:
		break;
	}
	return value;
}
