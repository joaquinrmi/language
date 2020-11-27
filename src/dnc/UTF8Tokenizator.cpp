#include "UTF8Tokenizator.hpp"

#include "UTF8Analyzer.hpp"
#include "TextStatus.hpp"

using namespace std;

namespace dnc
{
   const vector<CharType> UTF8Tokenizator::CHAR_TYPES({
      C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK,
      C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK,
      C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK,
      C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK, C_BLANK,
      C_BLANK, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL,
      C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL,
      C_NUMBER, C_NUMBER, C_NUMBER, C_NUMBER, C_NUMBER, C_NUMBER, C_NUMBER, C_NUMBER,
      C_NUMBER, C_NUMBER, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL,
      C_SYMBOL, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL,
      C_SYMBOL, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER, C_LETTER,
      C_LETTER, C_LETTER, C_LETTER, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_SYMBOL, C_BLANK
   });

   const uint8_t UTF8Tokenizator::NUMBER_POINT = '.';

   UTF8Tokenizator::UTF8Tokenizator()
   {}

   TextStatus UTF8Tokenizator::getToken(const string& text, uint32_t pos, TextToken& target)
   {
      int32_t char_count = 0;
      if(!UTF8Analyzer::countNextChar(text, char_count, pos))
      {
         return TextStatus("");
      }

      if(char_count == 1)
      {
         uint8_t c = text[pos];
         auto char_type = CHAR_TYPES[c];

         switch(char_type)
         {
         case C_BLANK:
         {
            target.type = TextToken::SPACE;
            target.value = text.substr(pos, char_count);

            auto pos2 = pos + 1;
            while(true)
            {
               int32_t char_count_2 = 0;
               if(!UTF8Analyzer::countNextChar(text, char_count_2, pos2))
               {
                  return TextStatus();
               }

               if(char_count_2 > 1)
               {
                  return TextStatus();
               }

               uint8_t c2 = text[pos2];
               if(CHAR_TYPES[c2] != C_BLANK)
               {
                  return TextStatus();
               }

               target.value += text.substr(pos2, char_count_2);
               ++pos2;
            }

            break;
         }

         case C_SYMBOL:
         {
            target.type = TextToken::SYMBOL;
            target.value = text.substr(pos, char_count);
            return TextStatus();
         }

         case C_NUMBER:
         {
            target.type = TextToken::NUMBER;
            target.value = text.substr(pos, char_count);

            bool added_point = false;

            auto pos2 = pos + 1;
            while(true)
            {
               int32_t char_count_2 = 0;
               if(!UTF8Analyzer::countNextChar(text, char_count_2, pos2))
               {
                  return TextStatus();
               }

               if(char_count_2 > 1)
               {
                  return TextStatus();
               }

               uint8_t c2 = text[pos2];
               if(CHAR_TYPES[c2] != C_NUMBER && CHAR_TYPES[c2] != C_SYMBOL)
               {
                  return TextStatus();
               }

               if(CHAR_TYPES[c2] == C_SYMBOL)
               {
                  if(c2 == NUMBER_POINT && !added_point)
                  {
                     added_point = true;
                  }
                  else return TextStatus();
               }

               target.value += text.substr(pos2, char_count_2);
               ++pos2;
            }

            break;
         }

         case C_LETTER:
            target.type = TextToken::WORD;
            target.value = text.substr(pos, char_count);

            auto pos2 = pos + 1;
            while(true)
            {
               int32_t char_count_2 = 0;
               if(!UTF8Analyzer::countNextChar(text, char_count_2, pos2))
               {
                  return TextStatus();
               }

               if(char_count_2 > 1)
               {
                  return TextStatus();
               }

               uint8_t c2 = text[pos2];
               if(CHAR_TYPES[c2] != C_LETTER)
               {
                  return TextStatus();
               }

               target.value += text.substr(pos2, char_count_2);
               ++pos2;
            }

            break;
         }
      }

      target.type = TextToken::SYMBOL;
      target.value = text.substr(pos, char_count);
      return TextStatus();
   }
}