#pragma once

#include <vector>
#include <map>
#include <unordered_set>
#include <string>

#include "LanguageExpression.hpp"

namespace dnc
{
   class Grammar : public LanguageExpression
   {
   public:
      Grammar();
      Grammar(const std::vector<LanguageExpression*>& expressions);
      ~Grammar();

      InitExpressionChar getInitExpressionChar() const override;

      void setExpressions(const std::vector<LanguageExpression*>& expressions);

      bool parse(const std::string& text, uint32_t& pos) const;
      bool parse(const std::string& text, uint32_t& pos, uint32_t last_pos) const;

      bool checkAndAdvance(const std::string& text, uint32_t& init_pos, uint32_t last_pos, bool ignore_rest) const override;

      void clear();

   private:
      std::map<std::string, std::unordered_set<LanguageExpression*>> terminal_ref;
      std::unordered_set<LanguageExpression*> nonterminal_ref;
      std::unordered_set<LanguageExpression*> any_terminal_ref;
   };
}