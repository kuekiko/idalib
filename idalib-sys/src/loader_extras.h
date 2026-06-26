#pragma once

#include "loader.hpp"

#include <cstdint>

#include "cxx.h"

uint64_t idalib_plugin_version(const plugin_t *p) {
  return p == nullptr ? 0 : p->version;
}

uint64_t idalib_plugin_flags(const plugin_t *p) {
  return p == nullptr ? 0 : p->flags;
}

bool idalib_save_database(const char *outfile, uint32_t flags) {
  return save_database(outfile != nullptr && outfile[0] != '\0' ? outfile : nullptr, flags);
}
