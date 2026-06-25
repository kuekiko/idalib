#pragma once

#include <typeinf.hpp>

bool idalib_declare_type(const char *decl, bool relaxed, bool replace) {
  tinfo_t tif;
  qstring name;
  int flags = PT_SIL | PT_SEMICOLON;
  if ( relaxed )
    flags |= PT_RELAXED;
  if ( replace )
    flags |= PT_REPLACE;

  if ( !parse_decl(&tif, &name, nullptr, decl, flags) )
    return false;
  if ( name.empty() )
    return false;

  int ntf_flags = NTF_TYPE;
  if ( replace )
    ntf_flags |= NTF_REPLACE;
  return tif.set_named_type(nullptr, name.c_str(), ntf_flags) == TERR_OK;
}

bool idalib_apply_decl_type(
        ea_t ea,
        const char *decl,
        bool relaxed,
        bool delay,
        bool strict) {
  tinfo_t tif;
  qstring name;
  int parse_flags = PT_SIL | PT_SEMICOLON | PT_VAR;
  if ( relaxed )
    parse_flags |= PT_RELAXED;

  if ( !parse_decl(&tif, &name, nullptr, decl, parse_flags) )
  {
    parse_flags &= ~PT_VAR;
    parse_flags |= PT_TYP;
    if ( !parse_decl(&tif, &name, nullptr, decl, parse_flags) )
      return false;
  }

  uint32 apply_flags = TINFO_DEFINITE;
  if ( delay )
    apply_flags |= TINFO_DELAYFUNC;
  if ( strict )
    apply_flags |= TINFO_STRICT;
  return apply_tinfo(ea, tif, apply_flags);
}
