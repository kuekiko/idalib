#pragma once

#include "auto.hpp"
#include "kernwin.hpp"

#include <cctype>
#include <string>
#include "pro.h"

#include <algorithm>
#include <array>
#include <cstdint>

struct license_manager_t;
struct license_manager_t_vtbl;

struct license_result_t {
  uint8_t lid[6];
  uint16_t _skip;
  uint32_t is_ok;
  uint32_t pidx;
  uint32_t eidx;
};

struct license_location_t {
  qstring server_host;
  uint16_t server_port;
  uint8_t _skip_a;
  uint8_t _skip_b;
  uint32_t _skip_c;
  uint64_t remote;
#if defined(__NT__)
  uint64_t _skip_d;
#endif
  qstring license_path;
  qstring license_path_pattern;
};

struct license_info_t {
  uint8_t lid[6];
  uint16_t _skip;
  uint32_t pidx;
  uint32_t eidx;
};

struct license_addon_info_t {
  uint8_t lid[6];
  uint16_t _skip_a;
  uint32_t aidx;
  uint8_t owner_lid[6];
  uint16_t _skip_b;
  uint32_t _skip_c;
  uint64_t start_date;
  uint64_t end_date;
};

struct license_manager_t_vtbl {
#if defined(__NT__)
  void *_skip_a[3];
#else
  void *_skip_a[4];
#endif
  int (*get_or_borrow_license)(license_manager_t *, void *, license_info_t *,
                               uint64_t, qstring *);
  void *(*get_license_location)(license_manager_t *);
  void *_skip_b[5];
  license_result_t *(*check)(license_manager_t *, bool *, int);
};

struct license_manager_t {
  license_manager_t_vtbl *_vtbl;
  qvector<qstring> ida_dirs;
  void *logger;
  license_location_t license_location;
  qvector<license_info_t> licenses;
  qstring user_name;
  qstring user_email;
  license_result_t result;
  uint32_t _skip_a[3];
  qstring owner;
  qvector<license_addon_info_t> addons;
  void *_skip_b[3];
  uint64_t start_date;
  uint64_t end_date;
  uint64_t issued_on;
  qstring description;
  uint64_t _skip_c;
  qstring license_content;
  uint64_t _skip_d[16];
  qstring machine_id;
};

struct config_t {
#if defined(__MACOS__)
  uint8_t _skip_a[0x258];
#elif defined(__LINUX__)
  uint8_t _skip_a[0x2a0];
#elif defined(__NT__)
  uint8_t _skip_a[0x240];
#endif
  license_location_t *license_location;
  license_info_t *license_info;
};

extern "C" license_manager_t *get_license_manager();
extern "C" config_t *get_current_config();

bool idalib_check_license() {
  auto manager = get_license_manager();
  if (!manager) {
    return false;
  }

  auto res = manager->_vtbl->check(manager, 0, 0);
  if (res && res->is_ok) {
    return true;
  }

  config_t *config = get_current_config();
  uint64_t flags = 16;

  // NOTE: this will contain a description of any error; we should likely
  // figure out how to expose it...
  qstring estr;

  auto nres = manager->_vtbl->get_or_borrow_license(
      manager, config->license_location, config->license_info, flags, &estr);

  return !nres;
}

bool idalib_get_license_id(std::array<uint8_t, 6> &id) {
  if (!idalib_check_license()) {
    return false;
  }

  auto manager = get_license_manager();
  if (!manager) {
    return false;
  }

  auto res = manager->_vtbl->check(manager, 0, 0);
  if (res && res->is_ok) {
    std::copy(std::begin(res->lid), std::end(res->lid), std::begin(id));
    return true;
  }

  return false;
}

static std::string idalib_quote_arg(const char *arg) {
  std::string value(arg);
  bool needs_quotes = value.empty();
  for (char ch : value) {
    if (std::isspace(static_cast<unsigned char>(ch))) {
      needs_quotes = true;
      break;
    }
  }
  if (!needs_quotes) {
    return value;
  }

  std::string out = "\"";
  for (char ch : value) {
    if (ch == '\\' || ch == '"') {
      out.push_back('\\');
    }
    out.push_back(ch);
  }
  out.push_back('"');
  return out;
}

int idalib_open_database_quiet(int argc, const char *const *argv, bool auto_analysis) {
  if (argc < 2) {
    return 2;
  }

  std::string args;
  for (int i = 1; i < argc - 1; ++i) {
    if (!args.empty()) {
      args.push_back(' ');
    }
    args += idalib_quote_arg(argv[i]);
  }

  return open_database(argv[argc - 1], auto_analysis, args.empty() ? nullptr : args.c_str());
}

rust::String idalib_ea2str(ea_t ea) {
  auto out = qstring();

  if (ea2str(&out, ea)) {
    return rust::String(out.c_str());
  } else {
    return rust::String();
  }
}
