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

	bool UTF8Analyzer::getCharCode(const string& utf8_chars, uint32_t pos, uint32_t& char_code)
	{
		if(pos >= utf8_chars.size()) return false;

		int char_large;
		if(!countNextChar(utf8_chars, char_large, pos))
		{
			return false;
		}

		if(char_large == 1)
		{
			char_code = utf8_chars[pos];
			return true;
		}
		else if(char_large == 2)
		{
			uint16_t c0 = utf8_chars[pos];
			uint16_t c1 = utf8_chars[pos + 1];

			c0 = c0 << 6;
			c0 = c0 & 0b0000011111000000;
			c1 = c1 & 0b0000000000111111;

			char_code = c0 | c1;
			return true;
		}
		else if(char_code == 3)
		{
			uint16_t c0 = utf8_chars[pos];
			uint16_t c1 = utf8_chars[pos + 1];
			uint16_t c2 = utf8_chars[pos + 2];

			c0 = c0 << 12;
			c1 = c1 << 6;

			c0 = c0 & 0b1111000000000000;
			c1 = c1 & 0b0000111111000000;
			c2 = c2 & 0b0000000000111111;

			char_code = c0 | c1 | c2;
			return true;
		}

		uint32_t c0 = utf8_chars[pos];
		uint32_t c1 = utf8_chars[pos + 1];
		uint32_t c2 = utf8_chars[pos + 2];
		uint32_t c3 = utf8_chars[pos + 3];

		c0 = c0 << 18;
		c1 = c1 << 12;
		c2 = c2 << 6;

		c0 = c0 & 0b000111000000000000000000;
		c1 = c1 & 0b000000111111000000000000;
		c2 = c2 & 0b000000000000111111000000;
		c3 = c3 & 0b000000000000000000111111;

		char_code = c0 | c1 | c2 | c3;
		return true;
	}
}