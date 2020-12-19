#include "Grammar.hpp"

#include "UTF8Analyzer.hpp"

using namespace std;

namespace dnc
{
   Grammar::Grammar()
   {}

   Grammar::Grammar(const vector<LanguageExpression*>& expressions)
   {
      setExpressions(expressions);
   }

   Grammar::~Grammar()
   {
      clear();
   }

   LanguageExpression::InitExpressionChar Grammar::getInitExpressionChar() const
   {
      return { ExpressionChar(this) };
   }

   void Grammar::setExpressions(const vector<LanguageExpression*>& expressions)
   {
      clear();

      for(uint32_t i = 0; i < expressions.size(); ++i)
      {
         auto init_char_vec = expressions[i]->getInitExpressionChar();
         for(auto& init_char : init_char_vec)
         {
            switch(init_char.type)
            {
            case ExpressionChar::TERMINAL:
            {
               auto& value = init_char.value;
               int char_count = 0;
               uint32_t pos = 0;
               while(UTF8Analyzer::countNextChar(value, char_count, pos))
               {
                  terminal_ref[value.substr(pos, char_count)].insert(expressions[i]);
                  pos += char_count;
               }
               break;
            }

            case ExpressionChar::NONTERMINAL:
               if(init_char.ref != this)
               {
                  clear();
                  return;
               }
               nonterminal_ref.insert(expressions[i]);
               break;

            case ExpressionChar::ANY_TERMINAL:
               any_terminal_ref.insert(expressions[i]);
               break;
            }
         }
      }
   }

   bool Grammar::parse(const string& text, uint32_t& pos) const
   {
      return parse(text, pos, text.size());
   }

   bool Grammar::parse(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= last_pos)
      {
         return true;
      }

      int char_count = 0;
      if(!UTF8Analyzer::countNextChar(text, char_count, pos))
      {
         return false;
      }

      auto terminal_found = terminal_ref.find(text.substr(pos, char_count));
      if(terminal_found == terminal_ref.end())
      {
         return false;
      }

      for(auto ref : terminal_found->second)
      {
         uint32_t current_pos = pos;
         if(!ref->check(text, current_pos))
         {
            continue;
         }

         if(current_pos >= last_pos)
         {
            return true;
         }

         bool repeat = false;
         while(true)
         {
            for(auto sec_ref : nonterminal_ref)
            {
               uint32_t sec_current_pos = current_pos;
               if(!sec_ref->jumpAndCheck(text, sec_current_pos, last_pos))
               {
                  continue;
               }

               if(sec_current_pos >= last_pos)
               {
                  return true;
               }
               else
               {
                  repeat = true;
                  break;
               }
            }

            if(!repeat) break;
         }

         return false;
      }

      return false;
   }

   bool Grammar::checkAndAdvance(const std::string& text, uint32_t& init_pos, uint32_t last_pos, bool ignore_rest) const
   {
      return parse(text, init_pos, last_pos);
   }

   void Grammar::clear()
   {
      terminal_ref.clear();
      nonterminal_ref.clear();
      any_terminal_ref.clear();
   }
}
