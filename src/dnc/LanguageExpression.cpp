#include "LanguageExpression.hpp"

#include "UTF8Analyzer.hpp"
#include "UTF8Tokenizator.hpp"

using namespace std;

namespace dnc
{
   const map<string, LanguageExpression::CommandCreator> LanguageExpression::COMMAND_CREATORS = {
      {"UCHAR", [](Command*& command, LanguageExpression::CommandArgs& args) -> bool {
         if(args.size() != 1)
         {
            return false;
         }

         if(args[0].char_count != 1)
         {
            return false;
         }

         command = new UCHARCommand(args[0].value);
         return true;
      }}
   };

   LanguageExpression::LanguageExpression()
   {}

   LanguageExpression::LanguageExpression(const string& expression)
   {
      create(expression, 0);
   }

   LanguageExpression::~LanguageExpression()
   {
      clear();
   }

   bool LanguageExpression::create(const string& expression, uint32_t init_pos)
   {
      return create(expression, init_pos, expression.size());
   }

   bool LanguageExpression::create(const string& expression, uint32_t init_pos, uint32_t last_pos)
   {
      if(last_pos > expression.size())
      {
         last_pos = expression.size();
      }

      uint32_t pos = init_pos;

      vector<Command*> current_command_sequence;
      while(pos < last_pos)
      {
         Command* command;
         if(!createCommand(command, expression, pos, last_pos))
         {
            for(auto c : current_command_sequence)
            {
               delete c;
            }

            return false;
         }

         current_command_sequence.push_back(command);
      }

      clear();
      command_sequence = move(current_command_sequence);

      return true;
   }

   void LanguageExpression::clear()
   {
      for(auto command : command_sequence)
      {
         delete command;
      }
   }

   string LanguageExpression::toString() const
   {
      string result;

      for(uint32_t i = 0; i < command_sequence.size(); ++i)
      {
         result += command_sequence[i]->toString();
      }

      return result;
   }

   bool LanguageExpression::createCommand(Command*& command, const string& expression, uint32_t& pos, uint32_t last_pos)
   {
      uint32_t command_begin = pos;

      while(true)
      {
         if(!(pos < last_pos) || !(pos < expression.size()))
         {
            return false;
         }

         if(expression[pos] == '(')
         {
            break;
         }

         ++pos;
      }

      string command_name = expression.substr(command_begin, pos - command_begin);
      ++pos;

      auto found = COMMAND_CREATORS.find(command_name);
      if(found == COMMAND_CREATORS.end())
      {
         return false;
      }

      CommandArgs args;
      if(!getCommandArgs(args, expression, pos, last_pos))
      {
         return false;
      }

      if(!found->second(command, args))
      {
         return false;
      }

      return true;
   }

   bool LanguageExpression::getCommandArgs(CommandArgs& args, const string& expression, uint32_t& pos, uint32_t last_pos)
   {
      if(pos >= last_pos || pos >= expression.size())
      {
         return false;
      }

      while(true)
      {
         TextToken token;
         if(!UTF8Tokenizator::getToken(expression, pos, token).ok())
         {
            return false;
         }
         pos += token.value.size();

         switch(token.type)
         {
         case TextToken::SPACE:
            continue;
            break;

         case TextToken::SYMBOL:
            if(token.value != "\"")
            {
               return false;
            }

            if(!getStringToken(args, expression, pos, last_pos))
            {
               return false;
            }

            break;

         case TextToken::WORD:
            return false;

         case TextToken::NUMBER:
            args.push_back({ CommandToken::NUMBER, token.value, token.char_count });
            break;
         }

         if(pos >= expression.size() || pos >= last_pos)
         {
            return false;
         }

         if(expression[pos] == ',')
         {
            ++pos;
            continue;
         }

         if(expression[pos] == ')')
         {
            ++pos;
            break;
         }

         return false;
      }

      return true;
   }

   bool LanguageExpression::getStringToken(CommandArgs& args, const string& expression, uint32_t& pos, uint32_t last_pos)
   {
      CommandToken command_token;
      command_token.type = CommandToken::STRING;
      command_token.char_count = 0;

      bool active_escape = false;
      while(true)
      {
         if(pos >= expression.size() || pos >= last_pos)
         {
            return false;
         }

         TextToken token;
         if(!UTF8Tokenizator::getToken(expression, pos, token).ok())
         {
            return false;
         }
         pos += token.value.size();

         if(token.type == TextToken::SYMBOL)
         {
            if(token.value == "\"")
            {
               if(active_escape)
               {
                  active_escape = false;
               }
               else
               {
                  break;
               }
            }
            else if(token.value == "\\")
            {
               if(active_escape)
               {
                  active_escape = false;
               }
               else
               {
                  active_escape = true;
                  continue;
               }
            }
         }
         else if(active_escape)
         {
            return false;
         }

         command_token.value += token.value;
         command_token.char_count += token.char_count;
      }

      args.push_back(command_token);

      return true;
   }

   /*
      class LanguageExpression::Command
   */
   LanguageExpression::Command::Command()
   {}

   LanguageExpression::Command::~Command()
   {}

   /*
      class LanguageExpression::UCHARCommand
   */
   LanguageExpression::UCHARCommand::UCHARCommand()
   {}

   LanguageExpression::UCHARCommand::UCHARCommand(const string& unique_char) :
      unique_char(unique_char)
   {}

   LanguageExpression::UCHARCommand::UCHARCommand(string&& unique_char) :
      unique_char(move(unique_char))
   {}

   LanguageExpression::UCHARCommand::~UCHARCommand()
   {}

   LanguageExpression::Command* LanguageExpression::UCHARCommand::copy() const
   {
      return new UCHARCommand(unique_char);
   }

   string LanguageExpression::UCHARCommand::toString() const
   {
      return string("UCHAR(") + unique_char + ")";
   }
}
