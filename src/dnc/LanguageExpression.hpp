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

      bool check(const std::string& text, uint32_t init_pos = 0) const;
      bool check(const std::string& text, uint32_t init_pos, uint32_t last_pos) const;

      void clear();

      std::string toString() const;

   private:
      class Command
      {
      public:
         Command();
         virtual ~Command();

         virtual bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const = 0;

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
         NUMCommand(uint8_t num);
         NUMCommand(uint8_t min_num, uint8_t max_num);
         ~NUMCommand();

         bool check(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

         Command* copy() const override;
         std::string toString() const override;

      private:
         uint8_t min_num;
         uint8_t max_num;
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