#include <memory>
#include <range.hpp>
#include <ua.hpp>
#include <xref.hpp>
#include <idp.hpp>

ea_t idalib_range_start_ea(const range_t *r) { return r->start_ea; }
ea_t idalib_range_end_ea(const range_t *r) { return r->end_ea; }
bool idalib_range_contains(const range_t *r, ea_t ea) { return r->contains(ea); }
std::size_t idalib_range_size(const range_t *r) { return static_cast<std::size_t>(r->size()); }

insn_t *idalib_decode_insn2(ea_t ea) {
  insn_t *insn = new insn_t();
  if ( decode_insn(insn, ea) <= 0 )
  {
    delete insn;
    return nullptr;
  }
  return insn;
}

void idalib_insn_free(insn_t *insn) { delete insn; }

ea_t idalib_insn_ea(const insn_t *insn) { return insn->ea; }
uint16 idalib_insn_itype(const insn_t *insn) { return insn->itype; }
uint16 idalib_insn_size(const insn_t *insn) { return insn->size; }
std::size_t idalib_insn_operand_count(const insn_t *insn) {
  for ( std::size_t i = 0; i < UA_MAXOP; ++i )
  {
    if ( insn->ops[i].type == o_void )
      return i;
  }
  return UA_MAXOP;
}

bool idalib_insn_operand(const insn_t *insn, std::size_t n, op_t *out) {
  if ( n >= UA_MAXOP )
    return false;
  *out = insn->ops[n];
  return out->type != o_void;
}

bool idalib_insn_is_basic_block_end(const insn_t *insn, bool call_stops_block) {
  return is_basic_block_end(*insn, call_stops_block);
}

bool idalib_insn_is_call(const insn_t *insn) { return is_call_insn(*insn); }
bool idalib_insn_is_indirect_jump(const insn_t *insn) { return is_indirect_jump_insn(*insn); }
bool idalib_insn_is_ret(const insn_t *insn, int flags) { return is_ret_insn(*insn, flags); }

xrefblk_t *idalib_xref_clone(const xrefblk_t *xref) { return new xrefblk_t(*xref); }

xrefblk_t *idalib_xref_first_from(ea_t from, int flags) {
  xrefblk_t *xref = new xrefblk_t();
  if ( !xrefblk_t_first_from(xref, from, flags) )
  {
    delete xref;
    return nullptr;
  }
  return xref;
}

xrefblk_t *idalib_xref_first_to(ea_t to, int flags) {
  xrefblk_t *xref = new xrefblk_t();
  if ( !xrefblk_t_first_to(xref, to, flags) )
  {
    delete xref;
    return nullptr;
  }
  return xref;
}

void idalib_xref_free(xrefblk_t *xref) { delete xref; }

bool idalib_xref_next_from2(xrefblk_t *xref) { return xrefblk_t_next_from(xref); }
bool idalib_xref_next_to2(xrefblk_t *xref) { return xrefblk_t_next_to(xref); }
ea_t idalib_xref_from(const xrefblk_t *xref) { return xref->from; }
ea_t idalib_xref_to(const xrefblk_t *xref) { return xref->to; }
bool idalib_xref_is_code(const xrefblk_t *xref) { return xref->iscode; }
uchar idalib_xref_type(const xrefblk_t *xref) { return xref->type; }
bool idalib_xref_user(const xrefblk_t *xref) { return xref->user; }
