#pragma once

#include <vector>
#include <map>
#include <unordered_set>
#include <string>
#include <functional>

#include "TextToken.hpp"
#include "ParseProduct.hpp"

namespace dnc
{
   class LanguageExpression
   {
   public:
      struct ExpressionChar
      {
         enum Type
         {
            TERMINAL,
            NONTERMINAL,
            ANY_TERMINAL
         };

         Type type;
         union
         {
            std::string value;
            const LanguageExpression* ref;
         };

         ExpressionChar();
         ExpressionChar(const std::string& value);
         ExpressionChar(const LanguageExpression* ref);
         ExpressionChar(const ExpressionChar& exp_char);
         ~ExpressionChar();
      };

      typedef std::vector<ExpressionChar> InitExpressionChar;

      class Command
      {
      public:
         Command();
         virtual ~Command();

         virtual bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const = 0;
         virtual bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const = 0;

         virtual InitExpressionChar getInitExpressionChar() const = 0;

         virtual Command* copy() const = 0;
         virtual std::string toString() const = 0;
      };

      struct CommandScope
      {
         std::function<bool(Command*&, const std::string&, uint32_t&, uint32_t)> createCommand;
         std::vector<const LanguageExpression*> expressions;
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

      typedef std::function<void(ParseProduct)> FactoryFunction;

      LanguageExpression();
      LanguageExpression(const std::string& text, const std::vector<const LanguageExpression*>& expressions = std::vector<const LanguageExpression*>());
      virtual ~LanguageExpression();

      virtual InitExpressionChar getInitExpressionChar() const;

      void setFactoryFunction(const FactoryFunction& func);
      void resetFactoryFunction();

      bool create(const std::string& text, uint32_t init_pos = 0, const std::vector<const LanguageExpression*>& expressions = std::vector<const LanguageExpression*>());
      bool create(const std::string& text, uint32_t init_pos, uint32_t last_pos, const std::vector<const LanguageExpression*>& expressions = std::vector<const LanguageExpression*>());

      virtual bool check(const std::string& text, uint32_t init_pos = 0, bool ignore_rest = true) const;
      virtual bool check(const std::string& text, uint32_t init_pos, uint32_t last_pos, bool ignore_rest = true) const;
      virtual bool checkAndAdvance(const std::string& text, uint32_t& init_pos, uint32_t last_pos, bool ignore_rest) const;

      virtual bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const;

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
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

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
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

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
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

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
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

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
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

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
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

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
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

         Command* copy() const override;
         std::string toString() const override;
      };

      class OPTBLANKCommand : public Command
      {
      public:
         OPTBLANKCommand();
         ~OPTBLANKCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

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
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

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
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

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
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         std::vector<Command*> first;
         std::vector<Command*> second;
      };

      class XORCommand : public Command
      {
      public:
         XORCommand();
         XORCommand(const std::vector<Command*>& first, const std::vector<Command*>& second);
         ~XORCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         std::vector<Command*> first;
         std::vector<Command*> second;
      };

      class OPTCommand : public Command
      {
      public:
         OPTCommand();
         OPTCommand(const std::vector<Command*>& sequence);
         ~OPTCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         std::vector<Command*> sequence;
      };

      class EXPCommand : public Command
      {
      public:
         EXPCommand();
         EXPCommand(const LanguageExpression* expression);
         ~EXPCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         const LanguageExpression* expression;
      };

      class RANGECommand : public Command
      {
      public:
         RANGECommand();
         RANGECommand(uint32_t min, uint32_t max);
         virtual ~RANGECommand();

         uint32_t getMin() const;
         uint32_t getMax() const;

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         uint32_t min;
         uint32_t max;
      };

      class LETTERCommand : public Command
      {
      public:
         LETTERCommand();
         ~LETTERCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         const uint32_t min0, max0;
         const uint16_t min1, max1;
      };

      class UPPERLETTERCommand : public RANGECommand
      {
      public:
         UPPERLETTERCommand();
         ~UPPERLETTERCommand();
      };

      class LOWERLETTERCommand : public RANGECommand
      {
      public:
         LOWERLETTERCommand();
         ~LOWERLETTERCommand();
      };

      class SETCommand : public Command
      {
      public:
         struct Range
         {
            uint32_t min;
            uint32_t max;
         };

         SETCommand();
         SETCommand(const std::string& chars);
         ~SETCommand();

         void addElement(uint32_t min, uint32_t max);
         void addFromString(const std::string& chars);

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
      	std::string chars;
         std::unordered_set<std::string> value;
         std::vector<Range> ranges;
      };

      class SWITCHCommand : public Command
      {
      public:
         SWITCHCommand();
         ~SWITCHCommand();

         void addCommand(Command* command);

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;
         bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         InitExpressionChar getInitExpressionChar() const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         std::unordered_set<Command*> commands;
      };

      typedef std::function<bool(Command*&, CommandArgs&, CommandScope&)> CommandCreator;

      static const std::map<std::string, CommandCreator> COMMAND_CREATORS;

      std::vector<Command*> command_sequence;
      CommandScope command_scope;
      bool has_factory_function;
      FactoryFunction factory_function;

      static bool checkCommands(const std::vector<Command*>& commands, const std::string& text, uint32_t& pos, uint32_t last_pos);

      bool createCommand(Command*& command, const std::string& text, uint32_t& pos, uint32_t last_pos);
      bool getCommandArgs(CommandArgs& args, const std::string& expression, uint32_t& pos, uint32_t last_pos);
      bool getStringToken(CommandArgs& args, const std::string& expression, uint32_t& pos, uint32_t last_pos);
      bool getCommandSequenceToken(CommandArgs& args, const std::string& expression, uint32_t& pos, uint32_t last_pos);
   };
}
