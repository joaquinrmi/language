#pragma once

#include <string>

namespace dnc
{
   class TextStatus
   {
   public:
      TextStatus();
      TextStatus(const std::string& message);

      bool ok() const noexcept;
      const std::string& what() const noexcept;

   private:
      bool status;
      std::string message;
   };
}