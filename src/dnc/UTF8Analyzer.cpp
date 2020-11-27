#include "UTF8Analyzer.hpp"

using namespace std;

namespace dnc
{
	const uint8_t UTF8Analyzer::ASCII_HEADER_MIN = 0;
	const uint8_t UTF8Analyzer::ASCII_HEADER_MAX = 128;
	const uint8_t UTF8Analyzer::TWO_BYTES_HEADER_MAX = 224;
	const uint8_t UTF8Analyzer::THREE_BYTES_HEADER_MAX = 240;
	const uint8_t UTF8Analyzer::FOUR_BYTES_HEADER_MAX = 247;
	const uint8_t UTF8Analyzer::NEXT_BYTE_HEADER_MIN = 128;
	const uint8_t UTF8Analyzer::NEXT_BYTE_HEADER_MAX = 192;

	UTF8Analyzer::UTF8Analyzer()
	{}

	bool UTF8Analyzer::countNextChar(const string& utf8_chars, int& target, uint32_t pos)
	{
		if(pos >= utf8_chars.size()) return false;
		
		uint8_t c = utf8_chars[pos];
		
		if(c < ASCII_HEADER_MAX)
		{
			target = 1;
			return true;
		}
		else if(c < TWO_BYTES_HEADER_MAX)
		{
			++pos;
			if(!readNextByte(utf8_chars, pos)) return false;
			target = 2;
			return true;
		}
		else if(c < THREE_BYTES_HEADER_MAX)
		{
			for(int j = 0; j < 2; ++j)
			{
				++pos;
				if(!readNextByte(utf8_chars, pos)) return false;
			}
			target = 3;
			return true;
		}
		else if(c < FOUR_BYTES_HEADER_MAX)
		{
			for(int j = 0; j < 3; ++j)
			{
				++pos;
				if(!readNextByte(utf8_chars, pos)) return false;
			}
			target = 4;
			return true;
		}

		return false;
	}

	bool UTF8Analyzer::readNextByte(const string& utf8_chars, uint32_t pos)
	{
		if(pos >= utf8_chars.size()) return false;

		uint8_t c = utf8_chars[pos];
		
		if(c >= NEXT_BYTE_HEADER_MIN && c < NEXT_BYTE_HEADER_MAX)
		{
			return true;
		}
		
		return false;
	}
}