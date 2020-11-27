#pragma once

#include <vector>
#include <map>
#include <string>
#include <functional>

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

      typedef std::function<bool(Command*&, const std::string&, uint32_t&, uint32_t)> CommandCreator;

      static const std::map<std::string, CommandCreator> COMMAND_CREATORS;

      std::vector<Command*> command_sequence;

      bool createCommand(Command*& command, const std::string& expression, uint32_t& pos, uint32_t last_pos);
   };
}