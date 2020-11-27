#pragma once

#include <string>

namespace dnc
{
   class TextToken
   {
   public:
      enum Type
      {
         SPACE,
         SYMBOL,
         WORD,
         NUMBER
      };

      TextToken();
      TextToken(Type type, const std::string& value);
      TextToken(Type type, std::string&& value);

      Type type;
      std::string value;
   };
}