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
      class Command
      {
      public:
         Command();
         virtual ~Command();

         virtual bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const = 0;

         virtual Command* copy() const = 0;
         virtual std::string toString() const = 0;
      };

      struct CommandScope
      {
         std::function<bool(Command*&, const std::string&, uint32_t&, uint32_t)> createCommand;
         std::vector<LanguageExpression*> expressions;
      };

      LanguageExpression();
      LanguageExpression(const std::string& expression);
      virtual ~LanguageExpression();

      bool create(const std::string& expression, uint32_t init_pos = 0);
      bool create(const std::string& expression, uint32_t init_pos, uint32_t last_pos);

      bool check(const std::string& text, uint32_t init_pos = 0) const;
      bool check(const std::string& text, uint32_t init_pos, uint32_t last_pos) const;

      void clear();

      std::string toString() const;

   private:
      class UCHARCommand : public Command
      {
      public:
         UCHARCommand();
         UCHARCommand(const std::string& unique_char);
         UCHARCommand(std::string&& unique_char);
         ~UCHARCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         std::string unique_char;
      };

      class CHARCommand : public Command
      {
      public:
         CHARCommand();
         ~CHARCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;
      };

      class STRCommand : public Command
      {
      public:
         STRCommand();
         STRCommand(const std::string& str);
         STRCommand(std::string&& str);
         ~STRCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         std::string value;
      };

      class NUMCommand : public Command
      {
      public:
         NUMCommand();
         NUMCommand(uint16_t num);
         NUMCommand(uint16_t min_num, uint16_t max_num);
         ~NUMCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         uint16_t min_num;
         uint16_t max_num;
      };

      class NUMTCommand : public Command
      {
      public:
         NUMTCommand();
         NUMTCommand(double num);
         NUMTCommand(double min_num, double max_num);
         ~NUMTCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         bool use_range;
         double min_num;
         double max_num;
      };

      class INUMTCommand : public Command
      {
      public:
         INUMTCommand();
         INUMTCommand(uint64_t num);
         INUMTCommand(uint64_t min_num, uint64_t max_num);
         ~INUMTCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         bool use_range;
         uint64_t min_num;
         uint64_t max_num;
      };

      class BLANKCommand : public Command
      {
      public:
         BLANKCommand();
         ~BLANKCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;
      };

      class OPTBLANKCommand : public Command
      {
      public:
         OPTBLANKCommand();
         ~OPTBLANKCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;
      };

      class REPCommand : public Command
      {
      public:
         REPCommand();
         REPCommand(const std::vector<Command*>& commands, uint32_t min = 1, uint32_t max = -1);
         virtual ~REPCommand();

         virtual bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         virtual Command* copy() const override;
         virtual std::string toString() const override;

      protected:
         std::vector<Command*> commands;
         uint32_t min;
         uint32_t max;
      };

      class REPIFCommand : public REPCommand
      {
      public:
         REPIFCommand();
         REPIFCommand(const std::vector<Command*>& sequence, const std::vector<Command*>& condition, bool ignore = false, uint32_t min = 1, uint32_t max = -1);
         ~REPIFCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         std::vector<Command*> condition;
         bool ignore;
      };

      class ORCommand : public Command
      {
      public:
         ORCommand();
         ORCommand(const std::vector<Command*>& first, const std::vector<Command*>& second);
         ~ORCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         std::vector<Command*> first;
         std::vector<Command*> second;
      };

      struct CommandToken
      {
         enum Type
         {
            BOOLEAN,
            STRING,
            NUMBER,
            COMMAND_SEQUENCE
         };

         Type type;
         std::string value;
         uint32_t char_count;
      };

      typedef std::vector<CommandToken> CommandArgs; 
      typedef std::function<bool(Command*&, CommandArgs&, CommandScope&)> CommandCreator;

      static const std::map<std::string, CommandCreator> COMMAND_CREATORS;

      std::vector<Command*> command_sequence;
      CommandScope command_scope;

      static bool checkCommands(const std::vector<Command*>& commands, const std::string& text, uint32_t& pos, uint32_t last_pos);

      bool createCommand(Command*& command, const std::string& expression, uint32_t& pos, uint32_t last_pos);
      bool getCommandArgs(CommandArgs& args, const std::string& expression, uint32_t& pos, uint32_t last_pos);
      bool getStringToken(CommandArgs& args, const std::string& expression, uint32_t& pos, uint32_t last_pos);
      bool getCommandSequenceToken(CommandArgs& args, const std::string& expression, uint32_t& pos, uint32_t last_pos);
   };
}
