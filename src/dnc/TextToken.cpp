#include "TextToken.hpp"

using namespace std;

namespace dnc
{
   TextToken::TextToken() :
      type(SPACE),
      value("")
   {}

   TextToken::TextToken(Type type, const string& value) :
      type(type),
      value(value)
   {}

   TextToken::TextToken(Type type, string&& value) :
      type(type),
      value(move(value))
   {}
}