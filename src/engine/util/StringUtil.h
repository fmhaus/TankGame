#pragma once

#include <string>
#include <sstream>

/// Utility for splitting strings by a delimiter
struct StringSplitter
{
	StringSplitter(const std::string& input, char delimiter)
		: ss(input), word(), delimiter(delimiter) 
	{}

	/// Returns the next word (or nullptr if not found)
	std::string* next_word()
	{
		if (std::getline(ss, word, delimiter))
			return &word;
		return nullptr;
	}

	std::stringstream ss;
	std::string word;
	char delimiter;
};
