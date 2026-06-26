#pragma once

#include <typeinf.hpp>

#include <cstdint>

#include "cxx.h"

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

size_t idalib_local_type_ordinal_limit() {
  return get_ordinal_limit(nullptr);
}

static bool idalib_get_numbered_type(tinfo_t *tif, uint32_t ordinal) {
  return tif != nullptr && tif->get_numbered_type(nullptr, ordinal);
}

static bool idalib_get_named_local_type(tinfo_t *tif, const char *name) {
  return tif != nullptr && name != nullptr && tif->get_named_type(nullptr, name);
}

static rust::String idalib_tinfo_name(const tinfo_t &tif) {
  qstring name;
  if (tif.get_type_name(&name))
    return rust::String(name.c_str());
  return rust::String();
}

static rust::String idalib_tinfo_decl(const tinfo_t &tif, const char *name) {
  qstring out;
  int flags = PRTYPE_MULTI | PRTYPE_TYPE | PRTYPE_DEF | PRTYPE_SEMI;
  if (tif.print(&out, name, flags))
    return rust::String(out.c_str());
  return rust::String(tif.dstr());
}

rust::String idalib_local_type_name(uint32_t ordinal) {
  tinfo_t tif;
  if (!idalib_get_numbered_type(&tif, ordinal))
    return rust::String();
  return idalib_tinfo_name(tif);
}

rust::String idalib_local_type_decl(uint32_t ordinal) {
  tinfo_t tif;
  if (!idalib_get_numbered_type(&tif, ordinal))
    return rust::String();
  rust::String name = idalib_tinfo_name(tif);
  return idalib_tinfo_decl(tif, name.empty() ? nullptr : name.c_str());
}

uint64_t idalib_local_type_size(uint32_t ordinal) {
  tinfo_t tif;
  if (!idalib_get_numbered_type(&tif, ordinal))
    return 0;
  return static_cast<uint64_t>(tif.get_size());
}

bool idalib_local_type_is_udt(uint32_t ordinal) {
  tinfo_t tif;
  return idalib_get_numbered_type(&tif, ordinal) && tif.is_udt();
}

bool idalib_local_type_is_union(uint32_t ordinal) {
  tinfo_t tif;
  if (!idalib_get_numbered_type(&tif, ordinal) || !tif.is_udt())
    return false;
  udt_type_data_t udt;
  return tif.get_udt_details(&udt) && udt.is_union;
}

bool idalib_local_type_is_enum(uint32_t ordinal) {
  tinfo_t tif;
  return idalib_get_numbered_type(&tif, ordinal) && tif.is_enum();
}

bool idalib_local_type_is_func(uint32_t ordinal) {
  tinfo_t tif;
  return idalib_get_numbered_type(&tif, ordinal) && tif.is_func();
}

bool idalib_local_type_is_ptr(uint32_t ordinal) {
  tinfo_t tif;
  return idalib_get_numbered_type(&tif, ordinal) && tif.is_ptr();
}

rust::String idalib_named_type_decl(const char *name) {
  tinfo_t tif;
  if (!idalib_get_named_local_type(&tif, name))
    return rust::String();
  return idalib_tinfo_decl(tif, name);
}

uint64_t idalib_named_type_size(const char *name) {
  tinfo_t tif;
  if (!idalib_get_named_local_type(&tif, name))
    return 0;
  return static_cast<uint64_t>(tif.get_size());
}

bool idalib_named_type_is_udt(const char *name) {
  tinfo_t tif;
  return idalib_get_named_local_type(&tif, name) && tif.is_udt();
}

bool idalib_named_type_is_union(const char *name) {
  tinfo_t tif;
  if (!idalib_get_named_local_type(&tif, name) || !tif.is_udt())
    return false;
  udt_type_data_t udt;
  return tif.get_udt_details(&udt) && udt.is_union;
}

size_t idalib_named_type_member_count(const char *name) {
  tinfo_t tif;
  if (!idalib_get_named_local_type(&tif, name) || !tif.is_udt())
    return 0;
  udt_type_data_t udt;
  if (!tif.get_udt_details(&udt))
    return 0;
  return udt.size();
}

static bool idalib_get_named_udt_member(udm_t *out, const char *name, size_t index) {
  tinfo_t tif;
  if (!idalib_get_named_local_type(&tif, name) || !tif.is_udt())
    return false;
  udt_type_data_t udt;
  if (!tif.get_udt_details(&udt) || index >= udt.size())
    return false;
  if (out != nullptr)
    *out = udt[index];
  return true;
}

rust::String idalib_named_type_member_name(const char *name, size_t index) {
  udm_t member;
  if (!idalib_get_named_udt_member(&member, name, index))
    return rust::String();
  return rust::String(member.name.c_str());
}

rust::String idalib_named_type_member_type(const char *name, size_t index) {
  udm_t member;
  if (!idalib_get_named_udt_member(&member, name, index))
    return rust::String();
  qstring out;
  if (member.type.print(&out, member.name.c_str(), PRTYPE_1LINE | PRTYPE_SEMI))
    return rust::String(out.c_str());
  return rust::String(member.type.dstr());
}

uint64_t idalib_named_type_member_offset(const char *name, size_t index) {
  udm_t member;
  if (!idalib_get_named_udt_member(&member, name, index))
    return 0;
  return member.offset / 8;
}

uint64_t idalib_named_type_member_size(const char *name, size_t index) {
  udm_t member;
  if (!idalib_get_named_udt_member(&member, name, index))
    return 0;
  return member.size / 8;
}

rust::String idalib_print_type_at(ea_t ea) {
  qstring out;
  if (print_type(&out, ea, PRTYPE_1LINE | PRTYPE_CPP | PRTYPE_SEMI))
    return rust::String(out.c_str());
  return rust::String();
}
