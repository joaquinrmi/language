#include "ParseProduct.hpp"

using namespace std;

namespace dnc
{
   ParseProduct::ParseProduct() :
      status(false),
      begin_pos(0),
      end_pos(0)
   {}

   ParseProduct::ParseProduct(uint32_t begin_pos, uint32_t end_pos) :
      status(true),
      begin_pos(begin_pos),
      end_pos(end_pos)
   {}

   ParseProduct::ParseProduct(uint32_t begin_pos, uint32_t end_pos, const vector<void*>& data) :
      data(data),
      status(true),
      begin_pos(begin_pos),
      end_pos(end_pos)
   {}

   bool ParseProduct::ok() const
   {
      return status;
   }

   uint32_t ParseProduct::begin() const
   {
      return begin_pos;
   }

   uint32_t ParseProduct::end() const
   {
      return end_pos;
   }

   bool ParseProduct::hasData() const
   {
      return data.size() > 0;
   }
}