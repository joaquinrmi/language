#pragma once

#include <vector>
#include <string>

#include "TextStatus.hpp"
#include "TextToken.hpp"

namespace dnc
{
   enum CharType
   {
      C_BLANK,
      C_SYMBOL,
      C_NUMBER,
      C_LETTER
   };
   
   class UTF8Tokenizator
   {
   public:
      static TextStatus getToken(const std::string& text, uint32_t pos, TextToken& target);

   private:
      static const std::vector<CharType> CHAR_TYPES;
      static const uint8_t NUMBER_POINT;

      UTF8Tokenizator();
   };
}
