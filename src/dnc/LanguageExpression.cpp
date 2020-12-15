#include "LanguageExpression.hpp"

#include <iostream>

#include "StringUtils.hpp"
#include "UTF8Analyzer.hpp"
#include "UTF8Tokenizator.hpp"

using namespace std;

namespace dnc
{
   bool createCommandSequence(vector<LanguageExpression::Command*>& sequence, LanguageExpression::CommandScope& scope, const std::string& text, uint32_t pos, uint32_t last_pos)
   {
      while(pos < text.size())
      {
       LanguageExpression::Command* current_command;
         if(!scope.createCommand(current_command, text, pos, last_pos))
         {
            for(auto c : sequence)
            {
               delete c;
            }

            return false;
         }

         sequence.push_back(current_command);
      }
     
     return true;
   }

   const map<string, LanguageExpression::CommandCreator> LanguageExpression::COMMAND_CREATORS = {
      {"UCHAR", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() != 1)
         {
            return false;
         }

         if(args[0].char_count != 1)
         {
            return false;
         }

         command = new UCHARCommand(move(args[0].value));
         return true;
      }},

      {"CHAR", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() != 0)
         {
            return false;
         }

         command = new CHARCommand();
         return true;
      }},

      {"STR", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() != 1)
         {
            return false;
         }

         command = new STRCommand(move(args[0].value));
         return true;
      }},

      {"NUM", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() == 0)
         {
            command = new NUMCommand(0, 9);
         }
         else if(args.size() == 1)
         {
            double num = atof(args[0].value.c_str());
            if((num / uint64_t(num)) - 1 > 0)
            {
               return false;
            }
            if(num < 0 || num > 9)
            {
               return false;
            }
            command = new NUMCommand(num);
            return true;
         }
         else if(args.size() == 2)
         {
            double num1 = atof(args[0].value.c_str());
            if((num1 / uint64_t(num1)) - 1 > 0)
            {
               return false;
            }
            if(num1 < 0 || num1 > 9)
            {
               return false;
            }
            double num2 = atof(args[1].value.c_str());
            if((num2 / uint64_t(num2)) - 1 > 0)
            {
               return false;
            }
            if(num2 < 0 || num2 > 9)
            {
               return false;
            }
            if(num2 < num1)
            {
               return false;
            }
            command = new NUMCommand(num1, num2);
            return true;
         }

         return false;
      }},

      {"NUMT", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() == 0)
         {
            command = new NUMTCommand();
            return true;
         }
         else if(args.size() == 1)
         {
            double num = atof(args[0].value.c_str());
            command = new NUMTCommand(num);
            return true;
         }
         else if(args.size() == 2)
         {
            double num1 = atof(args[0].value.c_str());
            double num2 = atof(args[1].value.c_str());
            command = new NUMTCommand(num1, num2);
            return true;
         }

         return false;
      }},

      {"INUMT", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() == 0)
         {
            command = new INUMTCommand();
            return true;
         }
         else if(args.size() == 1)
         {
            double num = atof(args[0].value.c_str());
            if((num / uint64_t(num)) - 1 > 0)
            {
               return false;
            }
            command = new INUMTCommand(num);
            return true;
         }
         else if(args.size() == 2)
         {
            double num1 = atof(args[0].value.c_str());
            if((num1 / uint64_t(num1)) - 1 > 0)
            {
               return false;
            }
            double num2 = atof(args[1].value.c_str());
            if((num2 / uint64_t(num2)) - 1 > 0)
            {
               return false;
            }
            command = new INUMTCommand(num1, num2);
            return true;
         }

         return false;
      }},

      {"_", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() != 0)
         {
            return false;
         }

         command = new BLANKCommand();
         return true;
      }},

      {"-", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() != 0)
         {
            return false;
         }

         command = new OPTBLANKCommand();
         return true;
      }},

      {"REP", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() == 0 || args.size() > 3)
         {
            return false;
         }

         vector<Command*> command_sequence;
         string& expression = args[0].value;
         uint32_t pos = 0;

         if(!createCommandSequence(command_sequence, scope, expression, pos, expression.size()))
         {
            return false;
         }

         if(args.size() == 1)
         {
            command = new REPCommand(command_sequence);
            return true;
         }
         else if(args.size() == 2)
         {
            command = new REPCommand(command_sequence, atof(args[1].value.c_str()));
            return true;
         }

         command = new REPCommand(command_sequence, atof(args[1].value.c_str()), atof(args[2].value.c_str()));
         return true;
      }},

      {"REPIF", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() < 2 || args.size() > 5)
         {
            return false;
         }

         vector<Command*> command_sequence;
         string& seq_expression = args[0].value;
         uint32_t pos = 0;

         if(!createCommandSequence(command_sequence, scope, seq_expression, pos, seq_expression.size()))
         {
            return false;
         }

         vector<Command*> condition_sequence;
         string& con_expression = args[1].value;

         if(!createCommandSequence(condition_sequence, scope, con_expression, pos, con_expression.size()))
         {
            return false;
         }

         if(args.size() == 2)
         {
            command = new REPIFCommand(command_sequence, condition_sequence);
            return true;
         }

         bool ignore = args[2].value == "true" ? true : false;

         if(args.size() == 3)
         {
            
            command = new REPIFCommand(command_sequence, condition_sequence, ignore);
            return true;
         }
         if(args.size() == 4)
         {
            command = new REPIFCommand(command_sequence, condition_sequence, ignore, atof(args[3].value.c_str()));
            return true;
         }

         command = new REPIFCommand(command_sequence, condition_sequence, ignore, atof(args[3].value.c_str()), atof(args[4].value.c_str()));
         return true;
      }},

      {"OR", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() != 2)
         {
            return false;
         }

         vector<Command*> first, second;
         string& first_expression = args[0].value;
         string& second_expression = args[1].value;
         uint32_t pos = 0;

         if(!createCommandSequence(first, scope, first_expression, pos, first_expression.size()))
         {
            return false;
         }

         if(!createCommandSequence(second, scope, second_expression, pos, second_expression.size()))
         {
            return false;
         }

         command = new ORCommand(first, second);
         return true;
      }},

      {"XOR", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() != 2)
         {
            return false;
         }

         vector<Command*> first, second;
         string& first_expression = args[0].value;
         string& second_expression = args[1].value;
         uint32_t pos = 0;

         if(!createCommandSequence(first, scope, first_expression, pos, first_expression.size()))
         {
            return false;
         }

         if(!createCommandSequence(second, scope, second_expression, pos, second_expression.size()))
         {
            return false;
         }

         command = new XORCommand(first, second);
         return true;
      }},

      {"OPT", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() != 1)
         {
            return false;
         }

         vector<Command*> sequence;
         string& expression = args[0].value;
         uint32_t pos = 0;

         if(!createCommandSequence(sequence, scope, expression, pos, expression.size()))
         {
            return false;
         }

         command = new OPTCommand(sequence);
         return true;
      }},

      {"EXP", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() != 1)
         {
            return false;
         }

         uint32_t exp_pos = atof(args[0].value.c_str());

         if(exp_pos >= scope.expressions.size())
         {
            return false;
         }

         command = new EXPCommand(scope.expressions[exp_pos]);
         return true;
      }},

      {"R", [](Command*& command, LanguageExpression::CommandArgs& args, LanguageExpression::CommandScope& scope) -> bool {
         if(args.size() != 2)
         {
            return false;
         }

         uint32_t min = atof(args[0].value.c_str());
         uint32_t max = atof(args[1].value.c_str());

         if(max < min)
         {
            return false;
         }

         command = new RANGECommand(min, max);
         return true;
      }},
   };

   LanguageExpression::LanguageExpression() :
      has_factory_function(false)
   {
      command_scope.createCommand = [this](Command*& a, const std::string& b, uint32_t& c, uint32_t d) -> bool
      {
         return this->createCommand(a, b, c, d);
      };
   }

   LanguageExpression::LanguageExpression(const string& expression, const vector<const LanguageExpression*>& expressions) :
      has_factory_function(false)
   {
      command_scope.createCommand = [this](Command*& a, const std::string& b, uint32_t& c, uint32_t d) -> bool
      {
         return this->createCommand(a, b, c, d);
      };
      create(expression, 0, expressions);
   }

   LanguageExpression::~LanguageExpression()
   {
      clear();
   }

   void LanguageExpression::setFactoryFunction(const FactoryFunction& func)
   {
      has_factory_function = true;
      factory_function = func;
   }

   void LanguageExpression::resetFactoryFunction()
   {
      has_factory_function = false;
   }

   bool LanguageExpression::create(const string& text, uint32_t init_pos, const vector<const LanguageExpression*>& expressions)
   {
      return create(text, init_pos, text.size(), expressions);
   }

   bool LanguageExpression::create(const string& text, uint32_t init_pos, uint32_t last_pos, const vector<const LanguageExpression*>& expressions)
   {
      command_scope.expressions = expressions;

      if(last_pos > text.size())
      {
         last_pos = text.size();
      }

      uint32_t pos = init_pos;

      vector<Command*> current_command_sequence;
      while(pos < last_pos)
      {
         Command* command;
         if(!createCommand(command, text, pos, last_pos))
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

   bool LanguageExpression::check(const string& text, uint32_t init_pos, bool ignore_rest) const
   {
      return checkAndAdvance(text, init_pos, text.size(), ignore_rest);
   }

   bool LanguageExpression::check(const string& text, uint32_t init_pos, uint32_t last_pos, bool ignore_rest) const
   {
      return checkAndAdvance(text, init_pos, last_pos, ignore_rest);
   }

   bool LanguageExpression::checkAndAdvance(const string& text, uint32_t& pos, uint32_t last_pos, bool ignore_rest) const
   {
      uint32_t init_pos = pos;

      for(auto command : command_sequence)
      {
         if(!command->check(text, pos, last_pos))
         {
            return false;
         }
      }

      if(ignore_rest || pos >= text.size())
      {
         if(has_factory_function)
         {
            factory_function(ParseProduct(init_pos, pos));
         }
         return true;
      }

      return false;
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

   bool LanguageExpression::checkCommands(const vector<Command*>& commands, const string& text, uint32_t& pos, uint32_t last_pos)
   {
      uint32_t current_pos = pos;
      for(uint32_t i = 0; i < commands.size(); ++i)
      {
         if(!commands[i]->check(text, current_pos, last_pos))
         {
            return false;
         }
      }

      pos = current_pos;
      return true;
   }

   bool LanguageExpression::createCommand(Command*& command, const string& text, uint32_t& pos, uint32_t last_pos)
   {
      uint32_t command_begin = pos;

      if(text[pos] == '-' || text[pos] == '_')
      {
         CommandArgs args;
         return COMMAND_CREATORS.at(text.substr(pos++, 1))(command, args, command_scope);
      }

      while(true)
      {
         if(!(pos < last_pos) || !(pos < text.size()))
         {
            return false;
         }

         if(text[pos] == '(')
         {
            break;
         }

         ++pos;
      }

      string command_name = text.substr(command_begin, pos - command_begin);
      ++pos;

      auto found = COMMAND_CREATORS.find(command_name);
      if(found == COMMAND_CREATORS.end())
      {
         return false;
      }

      CommandArgs args;
      if(!getCommandArgs(args, text, pos, last_pos))
      {
         return false;
      }

      if(!found->second(command, args, command_scope))
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
            if(token.value == ")")
            {
               return true;
            }

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
            if(token.value == "true" || token.value == "false")
            {
               args.push_back({ CommandToken::BOOLEAN, token.value, token.char_count });
               break;
            }

            pos -= token.value.size();

            if(!getCommandSequenceToken(args, expression, pos, last_pos))
            {
               return false;
            }

            break;

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

   bool LanguageExpression::getCommandSequenceToken(CommandArgs& args, const string& expression, uint32_t& pos, uint32_t last_pos)
   {
      CommandToken command_token;
      command_token.type = CommandToken::COMMAND_SEQUENCE;
      command_token.char_count = 0;

      uint32_t deep = 0;
      bool finished = false;
      uint32_t init_pos = pos;
      
      while(!finished)
      {
         if(pos >= last_pos || pos >= expression.size())
         {
            return false;
         }

         TextToken token;
         if(!UTF8Tokenizator::getToken(expression, pos, token).ok())
         {
            return false;
         }

         pos += token.char_count;

         switch(token.type)
         {
         case TextToken::SYMBOL:
            if(token.value == "(")
            {
               deep += 1;
               continue;
            }
            else if(token.value == ")")
            {
               if(deep == 0)
               {
                  finished = true;
                  break;
               }
               deep -= 1;
            }
            else if(token.value == ",")
            {
               if(deep == 0)
               {
                  finished = true;
                  break;
               }
            }
            continue;
            break;

         default:
            continue;
         }
      }

      pos -= 1;
      command_token.value = expression.substr(init_pos, pos - init_pos);
      command_token.char_count = pos - init_pos;
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

   bool LanguageExpression::UCHARCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      int char_count;
      if(!UTF8Analyzer::countNextChar(text, char_count, pos))
      {
         return false;
      }

      if(text.substr(pos, char_count) == unique_char)
      {
         pos += char_count;
         return true;
      }
      pos += char_count;

      return false;
   }

   LanguageExpression::Command* LanguageExpression::UCHARCommand::copy() const
   {
      return new UCHARCommand(unique_char);
   }

   string LanguageExpression::UCHARCommand::toString() const
   {
      return string("UCHAR(") + unique_char + ")";
   }

   /*
      class LanguageExpression::CHARCommand
   */
   LanguageExpression::CHARCommand::CHARCommand()
   {}

   LanguageExpression::CHARCommand::~CHARCommand()
   {}

   bool LanguageExpression::CHARCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      int char_count;
      if(!UTF8Analyzer::countNextChar(text, char_count, pos))
      {
         return false;
      }
      pos += char_count;

      if(char_count == 0)
      {
         return false;
      }

      return true;
   }

   LanguageExpression::Command* LanguageExpression::CHARCommand::copy() const
   {
      return new CHARCommand();
   }

   string LanguageExpression::CHARCommand::toString() const
   {
      return "CHAR()";
   }

   /*
      class LanguageExpression::STRCommand
   */
   LanguageExpression::STRCommand::STRCommand()
   {}

   LanguageExpression::STRCommand::STRCommand(const string& str) :
      value(str)
   {}

   LanguageExpression::STRCommand::STRCommand(string&& str) :
      value(move(str))
   {}

   LanguageExpression::STRCommand::~STRCommand()
   {}

   bool LanguageExpression::STRCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      uint32_t final_pos = pos + value.size();
      if(final_pos > text.size() || final_pos > last_pos)
      {
         return false;
      }

      if(text.substr(pos, value.size()) != value)
      {
         return false;
      }
      pos += value.size();

      return true;
   }

   LanguageExpression::Command* LanguageExpression::STRCommand::copy() const
   {
      return new STRCommand(value);
   }

   string LanguageExpression::STRCommand::toString() const
   {
      return string("STR(\"") + value + "\")";
   }

   /*
      class LanguageExpression::NUMCommand
   */
   LanguageExpression::NUMCommand::NUMCommand()
   {}

   LanguageExpression::NUMCommand::NUMCommand(uint16_t num) :
      min_num(num),
      max_num(num)
   {}

   LanguageExpression::NUMCommand::NUMCommand(uint16_t min, uint16_t max) :
      min_num(min),
      max_num(max)
   {}

   LanguageExpression::NUMCommand::~NUMCommand()
   {}

   bool LanguageExpression::NUMCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      TextToken token;
      if(!UTF8Tokenizator::getToken(text, pos, token).ok())
      {
         return false;
      }

      if(token.type != TextToken::NUMBER)
      {
         return false;
      }

      string str_num = token.value.substr(0, 1);
      double num = atof(str_num.c_str());

      pos += 1;

      if(num >= min_num && num <= max_num)
      {
         return true;
      }

      return false;
   }

   LanguageExpression::Command* LanguageExpression::NUMCommand::copy() const
   {
      return new NUMCommand(min_num, max_num);
   }

   string LanguageExpression::NUMCommand::toString() const
   {
      if(max_num == min_num)
      {
         return string("NUM(") + dnc::toString(min_num) + ")";
      }
     
     return string("NUM(") + dnc::toString(min_num) + "," + dnc::toString(max_num) + ")";
   }

   /*
      class LanguageExpression::NUMTCommand
   */
   LanguageExpression::NUMTCommand::NUMTCommand() :
      use_range(false)
   {}

   LanguageExpression::NUMTCommand::NUMTCommand(double num) :
      use_range(true),
      min_num(num),
      max_num(num)
   {}

   LanguageExpression::NUMTCommand::NUMTCommand(double min, double max) :
      use_range(true),
      min_num(min),
      max_num(max)
   {}

   LanguageExpression::NUMTCommand::~NUMTCommand()
   {}

   bool LanguageExpression::NUMTCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      TextToken token;
      if(!UTF8Tokenizator::getToken(text, pos, token).ok())
      {
         return false;
      }

      if(token.type != TextToken::NUMBER)
      {
         return false;
      }

      pos += token.char_count;

      if(!use_range)
      {
         return true;
      }

      double num = atof(token.value.c_str());

      if(num >= min_num && num <= max_num)
      {
         return true;
      }

      return false;
   }

   LanguageExpression::Command* LanguageExpression::NUMTCommand::copy() const
   {
      if(use_range)
      {
         return new NUMTCommand(min_num, max_num);
      }

      return new NUMTCommand();
   }

   string LanguageExpression::NUMTCommand::toString() const
   {
      if(!use_range)
      {
         return "NUMT()";
      }

      if(max_num == min_num)
      {
         return string("NUMT(") + dnc::toString(min_num) + ")";
      }
     
     return string("NUMT(") + dnc::toString(min_num) + "," + dnc::toString(max_num) + ")";
   }

   /*
      class LanguageExpression::INUMTCommand
   */
   LanguageExpression::INUMTCommand::INUMTCommand() :
      use_range(false)
   {}

   LanguageExpression::INUMTCommand::INUMTCommand(uint64_t num) :
      use_range(true),
      min_num(num),
      max_num(num)
   {}

   LanguageExpression::INUMTCommand::INUMTCommand(uint64_t min, uint64_t max) :
      use_range(true),
      min_num(min),
      max_num(max)
   {}

   LanguageExpression::INUMTCommand::~INUMTCommand()
   {}

   bool LanguageExpression::INUMTCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      TextToken token;
      if(!UTF8Tokenizator::getToken(text, pos, token).ok())
      {
         return false;
      }

      if(token.type != TextToken::NUMBER)
      {
         return false;
      }

      pos += token.char_count;

      double num = atof(token.value.c_str());
      if((num / uint64_t(num)) - 1 > 0)
      {
         return false;
      }

      if(!use_range)
      {
         return true;
      }

      if(num >= min_num && num <= max_num)
      {
         return true;
      }

      return false;
   }

   LanguageExpression::Command* LanguageExpression::INUMTCommand::copy() const
   {
      if(use_range)
      {
         return new INUMTCommand(min_num, max_num);
      }

      return new INUMTCommand();
   }

   string LanguageExpression::INUMTCommand::toString() const
   {
      if(!use_range)
      {
         return "INUMT()";
      }

      if(max_num == min_num)
      {
         return string("INUMT(") + dnc::toString(min_num) + ")";
      }
     
     return string("INUMT(") + dnc::toString(min_num) + "," + dnc::toString(max_num) + ")";
   }

   /*
      class LanguageExpression::BLANKCommand
   */
   LanguageExpression::BLANKCommand::BLANKCommand()
   {}

   LanguageExpression::BLANKCommand::~BLANKCommand()
   {}

   bool LanguageExpression::BLANKCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      bool found = false;

      while(true)
      {
         TextToken token;
         if(!UTF8Tokenizator::getToken(text, pos, token).ok())
         {
            break;
         }

         if(token.type != TextToken::SPACE)
         {
            break;
         }

         pos += token.value.size();
         found = true;
      }

      return found;
   }

   LanguageExpression::Command* LanguageExpression::BLANKCommand::copy() const
   {
      return new BLANKCommand();
   }

   string LanguageExpression::BLANKCommand::toString() const
   {
      return "_";
   }

   /*
      class LanguageExpression::OPTBLANKCommand
   */
   LanguageExpression::OPTBLANKCommand::OPTBLANKCommand()
   {}

   LanguageExpression::OPTBLANKCommand::~OPTBLANKCommand()
   {}

   bool LanguageExpression::OPTBLANKCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      while(true)
      {
         TextToken token;
         if(!UTF8Tokenizator::getToken(text, pos, token).ok())
         {
            break;
         }

         if(token.type != TextToken::SPACE)
         {
            break;
         }

         pos += token.value.size();
      }

      return true;
   }

   LanguageExpression::Command* LanguageExpression::OPTBLANKCommand::copy() const
   {
      return new OPTBLANKCommand();
   }

   string LanguageExpression::OPTBLANKCommand::toString() const
   {
      return "-";
   }

   /*
      class LanguageExpression::REPCommand
   */
   LanguageExpression::REPCommand::REPCommand()
   {}

   LanguageExpression::REPCommand::REPCommand(const vector<Command*>& commands, uint32_t min, uint32_t max) :
      commands(commands),
      min(min),
      max(max)
   {}

   LanguageExpression::REPCommand::~REPCommand()
   {
      for(auto c : commands)
      {
         delete c;
      }
   }

   bool LanguageExpression::REPCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      uint32_t repeated = 0;
      uint32_t current_pos = pos;
      while(true)
      {
         if(!checkCommands(commands, text, current_pos, last_pos))
         {
            break;
         }

         repeated += 1;
      }

      pos = current_pos;

      if(repeated >= min && repeated <= max)
      {
         return true;
      }

      return false;
   }

   LanguageExpression::Command* LanguageExpression::REPCommand::copy() const
   {
      vector<Command*> copied_commands;
      for(auto c : commands)
      {
         copied_commands.push_back(c->copy());
      }

      return new REPCommand(copied_commands, min, max);
   }

   string LanguageExpression::REPCommand::toString() const
   {
      string commands_str;
      for(auto c : commands)
      {
         commands_str += c->toString();
      }

      string result = string("REP(") + commands_str;
      if(min != 1 || max != uint32_t(-1))
      {
         result += "," + dnc::toString(min);
      }
      if(max != uint32_t(-1))
      {
         result += "," + dnc::toString(max);
      }
      result += ")";

      return result;
   }

   /*
      class LanguageExpression::REPIFCommand
   */
   LanguageExpression::REPIFCommand::REPIFCommand()
   {}

   LanguageExpression::REPIFCommand::REPIFCommand(const vector<Command*>& sequence, const vector<Command*>& condition, bool ignore, uint32_t min, uint32_t max) :
      REPCommand(sequence, min, max),
      condition(condition),
      ignore(ignore)
   {}

   LanguageExpression::REPIFCommand::~REPIFCommand()
   {
      for(auto c : condition)
      {
         delete c;
      }
   }

   bool LanguageExpression::REPIFCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      uint32_t repeated = 0;
      uint32_t current_pos = pos;
      bool condition_found = false;
      while(true)
      {
         if(!checkCommands(commands, text, current_pos, last_pos))
         {
            if(condition_found && !ignore)
            {
               return false;
            }
            break;
         }

         repeated += 1;

         if(!checkCommands(condition, text, current_pos, last_pos))
         {
            break;
         }
         condition_found = true;
      }

      pos = current_pos;

      if(repeated >= min && repeated <= max)
      {
         return true;
      }

      return false;
   }

   LanguageExpression::Command* LanguageExpression::REPIFCommand::copy() const
   {
      vector<Command*> copied_sequence;
      for(auto c : commands)
      {
         copied_sequence.push_back(c->copy());
      }

      vector<Command*> copied_condition;
      for(auto c : condition)
      {
         copied_condition.push_back(c->copy());
      }

      return new REPIFCommand(copied_sequence, condition, ignore, min, max);
   }

   string LanguageExpression::REPIFCommand::toString() const
   {
      string sequence_str;
      for(auto c : commands)
      {
         sequence_str += c->toString();
      }

      string condition_str;
      for(auto c : condition)
      {
         condition_str += c->toString();
      }

      string result = string("REPIF(") + sequence_str + "," + condition_str;
      if(ignore)
      {
         result += ",true";
      }
      if(min != 1 || max != uint32_t(-1))
      {
         if(!ignore)
         {
            result += ",false";
         }
         result += "," + dnc::toString(min);
      }
      if(max != uint32_t(-1))
      {
         result += "," + dnc::toString(max);
      }
      result += ")";

      return result;
   }

   /*
      class LanguageExpression::ORCommand
   */
   LanguageExpression::ORCommand::ORCommand()
   {}

   LanguageExpression::ORCommand::ORCommand(const vector<Command*>& first, const vector<Command*>& second) :
      first(first),
      second(second)
   {}

   LanguageExpression::ORCommand::~ORCommand()
   {
      for(auto c : first)
      {
         delete c;
      }

      for(auto c : second)
      {
         delete c;
      }
   }

   bool LanguageExpression::ORCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      if(checkCommands(first, text, pos, last_pos))
      {
         checkCommands(second, text, pos, last_pos);
         return true;
      }

      if(checkCommands(second, text, pos, last_pos))
      {
         checkCommands(first, text, pos, last_pos);
         return true;
      }

      return false;
   }

   LanguageExpression::Command* LanguageExpression::ORCommand::copy() const
   {
      return new ORCommand(first, second);
   }

   string LanguageExpression::ORCommand::toString() const
   {
      string first_str;
      for(auto c : first)
      {
         first_str += c->toString();
      }

      string second_str;
      for(auto c : second)
      {
         second_str += c->toString();
      }

      return string("OR(") + first_str + "," + second_str + ")";
   }

   /*
      class LanguageExpression::XORCommand
   */
   LanguageExpression::XORCommand::XORCommand()
   {}

   LanguageExpression::XORCommand::XORCommand(const vector<Command*>& first, const vector<Command*>& second) :
      first(first),
      second(second)
   {}

   LanguageExpression::XORCommand::~XORCommand()
   {
      for(auto c : first)
      {
         delete c;
      }

      for(auto c : second)
      {
         delete c;
      }
   }

   bool LanguageExpression::XORCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      if(checkCommands(first, text, pos, last_pos))
      {
         return true;
      }

      if(checkCommands(second, text, pos, last_pos))
      {
         return true;
      }

      return false;
   }

   LanguageExpression::Command* LanguageExpression::XORCommand::copy() const
   {
      return new XORCommand(first, second);
   }

   string LanguageExpression::XORCommand::toString() const
   {
      string first_str;
      for(auto c : first)
      {
         first_str += c->toString();
      }

      string second_str;
      for(auto c : second)
      {
         second_str += c->toString();
      }

      return string("XOR(") + first_str + "," + second_str + ")";
   }

   /*
      class LanguageExpression::OPTCommand
   */
   LanguageExpression::OPTCommand::OPTCommand()
   {}

   LanguageExpression::OPTCommand::OPTCommand(const vector<Command*>& sequence) :
      sequence(sequence)
   {}

   LanguageExpression::OPTCommand::~OPTCommand()
   {
      for(auto c : sequence)
      {
         delete c;
      }
   }

   bool LanguageExpression::OPTCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      if(checkCommands(sequence, text, pos, last_pos))
      {
         return true;
      }

      return true;
   }

   LanguageExpression::Command* LanguageExpression::OPTCommand::copy() const
   {
      return new OPTCommand(sequence);
   }

   string LanguageExpression::OPTCommand::toString() const
   {
      string sequence_str;
      for(auto c : sequence)
      {
         sequence_str += c->toString();
      }

      return string("OPT(") + sequence_str + ")";
   }

   /*
      class LanguageExpression::EXPCommand
   */
   LanguageExpression::EXPCommand::EXPCommand()
   {}

   LanguageExpression::EXPCommand::EXPCommand(const LanguageExpression* expression) :
      expression(expression)
   {}

   LanguageExpression::EXPCommand::~EXPCommand()
   {}

   bool LanguageExpression::EXPCommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      return expression->checkAndAdvance(text, pos, last_pos, true);
   }

   LanguageExpression::Command* LanguageExpression::EXPCommand::copy() const
   {
      return new EXPCommand(expression);
   }

   string LanguageExpression::EXPCommand::toString() const
   {
      return expression->toString();
   }

   /*
      class LanguageExpression::RANGECommand
   */
   LanguageExpression::RANGECommand::RANGECommand()
   {}

   LanguageExpression::RANGECommand::RANGECommand(uint32_t min, uint32_t max) :
      min(min),
      max(max)
   {}

   LanguageExpression::RANGECommand::~RANGECommand()
   {}

   bool LanguageExpression::RANGECommand::check(const string& text, uint32_t& pos, uint32_t last_pos) const
   {
      if(pos >= text.size() || pos >= last_pos)
      {
         return false;
      }

      uint32_t char_code;
      if(!UTF8Analyzer::getCharCode(text, pos, char_code))
      {
         return false;
      }

      if(char_code < min || char_code > max)
      {
         return false;
      }

      int char_count;
      if(!UTF8Analyzer::countNextChar(text, char_count, pos))
      {
         return false;
      }

      pos += char_count;

      return true;
   }

   LanguageExpression::Command* LanguageExpression::RANGECommand::copy() const
   {
      return new RANGECommand(min, max);
   }

   string LanguageExpression::RANGECommand::toString() const
   {
      return string("R(") + dnc::toString(min) + "," + dnc::toString(max) + ")";
   }
}
