#pragma once

#include <cstdint>
#include <vector>

namespace dnc
{
   class ParseProduct
   {
   public:
      ParseProduct();
      ParseProduct(uint32_t begin_pos, uint32_t end_pos);

      bool ok() const;
      uint32_t begin() const;
      uint32_t end() const;

   private:
      bool status;
      uint32_t begin_pos;
      uint32_t end_pos;
   };
}