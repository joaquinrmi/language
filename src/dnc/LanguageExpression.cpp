#include "LanguageExpression.hpp"

using namespace std;

namespace dnc
{
   const map<string, LanguageExpression::CommandCreator> LanguageExpression::COMMAND_CREATORS = {
      {"UCHAR", [](Command*& command, const string& expression, uint32_t& pos, uint32_t last_pos) -> bool {
         command = new UCHARCommand("0");
         pos += 2;
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

      auto found = COMMAND_CREATORS.find(command_name);
      if(found == COMMAND_CREATORS.end())
      {
         return false;
      }

      return found->second(command, expression, ++pos, last_pos);
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