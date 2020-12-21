#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>

#include "LanguageExpression.hpp"

namespace dnc
{
   class Grammar : public LanguageExpression
   {
   public:
      Grammar();
      Grammar(const std::vector<const LanguageExpression*>& expressions);
      ~Grammar();

      InitExpressionChar getInitExpressionChar() const override;

      void setExpressions(const std::vector<const LanguageExpression*>& expressions);

      bool parse(const std::string& text, uint32_t& pos) const;
      bool parse(const std::string& text, uint32_t& pos, uint32_t last_pos) const;

      bool checkAndAdvance(const std::string& text, uint32_t& init_pos, uint32_t last_pos, bool ignore_rest) const override;

      bool jumpAndCheck(const std::string& text, uint32_t& pos, uint32_t last_pos) const override;

      void clear();

   private:
      /*
         Este objeto es temporal
      */
      LanguageExpression HERO_EXPRESSION;

      std::unordered_map<std::string, std::set<const LanguageExpression*>> terminal_ref;
      std::unordered_set<const LanguageExpression*> nonterminal_ref;
      std::unordered_set<const LanguageExpression*> any_terminal_ref;
   };
}