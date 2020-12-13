#pragma once

#include <cstdint>
#include <vector>

namespace dnc
{
   class ParseProduct
   {
   public:
      std::vector<void*> data;

      ParseProduct();
      ParseProduct(uint32_t begin_pos, uint32_t end_pos);
      ParseProduct(uint32_t begin_pos, uint32_t end_pos, const std::vector<void*>& data);

      bool ok() const;
      uint32_t begin() const;
      uint32_t end() const;
      bool hasData() const;

   private:
      bool status;
      uint32_t begin_pos;
      uint32_t end_pos;
   };
}