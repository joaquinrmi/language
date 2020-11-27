#pragma once

#include <vector>
#include <map>
#include <string>
#include <functional>

#include "TextToken.hpp"

namespace dnc
{
   class LanguageExpression
   {
   public:
      LanguageExpression();
      LanguageExpression(const std::string& expression);
      virtual ~LanguageExpression();

      bool create(const std::string& expression, uint32_t init_pos = 0);
      bool create(const std::string& expression, uint32_t init_pos, uint32_t last_pos);

      void clear();

      std::string toString() const;

   private:
      class Command
      {
      public:
         Command();
         virtual ~Command();

         virtual Command* copy() const = 0;
         virtual std::string toString() const = 0;
      };

      class UCHARCommand : public Command
      {
      public:
         UCHARCommand();
         UCHARCommand(const std::string& unique_char);
         UCHARCommand(std::string&& unique_char);
         ~UCHARCommand();

         Command* copy() const override;
         std::string toString() const override;

      private:
         std::string unique_char;
      };

      struct CommandToken
      {
         enum Type
         {
            STRING,
            NUMBER
         };

         Type type;
         std::string value;
         uint32_t char_count;
      };

      typedef std::vector<CommandToken> CommandArgs; 
      typedef std::function<bool(Command*&, CommandArgs&)> CommandCreator;

      static const std::map<std::string, CommandCreator> COMMAND_CREATORS;

      std::vector<Command*> command_sequence;

      bool createCommand(Command*& command, const std::string& expression, uint32_t& pos, uint32_t last_pos);
      bool getCommandArgs(CommandArgs& args, const std::string& expression, uint32_t& pos, uint32_t last_pos);
      bool getStringToken(CommandArgs& args, const std::string& expression, uint32_t& pos, uint32_t last_pos);
   };
}