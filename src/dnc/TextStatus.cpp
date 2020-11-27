#include "TextStatus.hpp"

using namespace std;

namespace dnc
{
   TextStatus::TextStatus() :
      status(true)
   {}
   
   TextStatus::TextStatus(const string& message) :
      status(false),
      message(message)
   {}

   bool TextStatus::ok() const noexcept
   {
      return status;
   }

   const string& TextStatus::what() const noexcept
   {
      return message;
   }
}