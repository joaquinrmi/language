#pragma once

#include <string>
#include <sstream>

namespace dnc
{
	template<typename T>
	std::string toString(const T& element)
	{
		std::stringstream s;
		s << element;
		return s.str();
	}

	template<>
	inline std::string toString(const std::string& element)
	{
		return element;
	}
}