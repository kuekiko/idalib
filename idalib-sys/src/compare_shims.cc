#include <pro.h>
#include <funcs.hpp>
#include <segment.hpp>

int func_t::compare(const func_t &r) const
{
  return range_t::compare(r);
}

int segment_t::compare(const segment_t &r) const
{
  return range_t::compare(r);
}
