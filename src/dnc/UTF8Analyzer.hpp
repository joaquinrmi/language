#pragma once

#include <string>

namespace dnc
{
   class UTF8Analyzer
   {
   public:
      static bool countNextChar(const std::string& utf8_chars, int& target, uint32_t pos);
      static bool readNextByte(const std::string& utf8_chars, uint32_t pos);
      static bool getCharCode(const std::string& utf8_chars, uint32_t pos, uint32_t& char_code);
      static std::string getChar(uint32_t char_code);

   private:
      UTF8Analyzer();

      static const uint8_t ASCII_HEADER_MIN;
      static const uint8_t ASCII_HEADER_MAX;
      static const uint8_t TWO_BYTES_HEADER_MAX;
      static const uint8_t THREE_BYTES_HEADER_MAX;
      static const uint8_t FOUR_BYTES_HEADER_MAX;
      static const uint8_t NEXT_BYTE_HEADER_MIN;
      static const uint8_t NEXT_BYTE_HEADER_MAX;
   };
}