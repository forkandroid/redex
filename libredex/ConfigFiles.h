/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_set>

#include <json/json.h>

#include "DexClass.h"
#include "InlinerConfig.h"
#include "ProguardMap.h"

class DexType;
using MethodTuple = std::tuple<DexString*, DexString*, DexString*>;
using MethodMap = std::map<MethodTuple, DexClass*>;

class JsonWrapper {
 public:
  JsonWrapper() = default;

  explicit JsonWrapper(const Json::Value& config) : m_config(config) {}

  void get(const char* name, int64_t dflt, int64_t& param) const;

  void get(const char* name, size_t dflt, size_t& param) const;

  void get(const char* name, const std::string& dflt, std::string& param) const;

  const std::string get(const char* name, const std::string& dflt) const;

  void get(const char* name, bool dflt, bool& param) const;

  bool get(const char* name, bool dflt) const;

  void get(const char* name,
           const std::vector<std::string>& dflt,
           std::vector<std::string>& param) const;

  void get(const char* name,
           const std::vector<std::string>& dflt,
           std::unordered_set<std::string>& param) const;

  void get(
      const char* name,
      const std::unordered_map<std::string, std::vector<std::string>>& dflt,
      std::unordered_map<std::string, std::vector<std::string>>& param) const;

  void get(const char* name, const Json::Value dflt, Json::Value& param) const;

  const Json::Value get(const char* name, const Json::Value dflt) const;

  const Json::Value& operator[](const char* name) const;

 private:
  Json::Value m_config;
};

/**
 * ConfigFiles should be a readonly structure
 */
struct ConfigFiles {
  ConfigFiles(const Json::Value& config);
  ConfigFiles(const Json::Value& config, const std::string& outdir);

  const std::vector<std::string>& get_coldstart_classes() {
    if (m_coldstart_classes.size() == 0) {
      m_coldstart_classes = load_coldstart_classes();
    }
    return m_coldstart_classes;
  }

  const std::vector<std::string>& get_coldstart_methods() {
    if (m_coldstart_methods.size() == 0) {
      m_coldstart_methods = load_coldstart_methods();
    }
    return m_coldstart_methods;
  }

  void ensure_class_lists_loaded() {
    if (!m_load_class_lists_attempted) {
      m_load_class_lists_attempted = true;
      m_class_lists = load_class_lists();
    }
  }

  const std::unordered_map<std::string, std::vector<std::string> >& get_all_class_lists() {
    ensure_class_lists_loaded();
    return m_class_lists;
  }

  const bool has_class_list(const std::string& name) {
    ensure_class_lists_loaded();
    return m_class_lists.count(name) != 0;
  }

  const std::vector<std::string>& get_class_list(const std::string& name) {
    ensure_class_lists_loaded();
    return m_class_lists.at(name);
  }

  const std::unordered_set<DexType*>& get_no_optimizations_annos();

  const std::unordered_map<std::string, unsigned int>& get_method_to_weight()
      const {
    return m_method_to_weight;
  }

  const std::unordered_set<std::string>&
  get_method_sorting_whitelisted_substrings() const {
    return m_method_sorting_whitelisted_substrings;
  }

  bool save_move_map() const { return m_move_map; }

  const MethodMap& get_moved_methods_map() const {
    return m_moved_methods_map;
  }

  /* DEPRECATED! */
  void add_moved_methods(MethodTuple mt, DexClass* cls) {
    m_move_map = true;
    m_moved_methods_map[mt] = cls;
  }

  std::string metafile(const std::string& basename) const {
    if (basename.empty()) {
      return std::string();
    }
    return outdir + '/' + basename;
  }

  std::string get_outdir() const {
    return outdir;
  }

  const ProguardMap& get_proguard_map() const {
    return m_proguard_map;
  }

  const std::string& get_printseeds() const { return m_printseeds; }

  uint32_t get_instruction_size_bitwidth_limit() const {
    return m_instruction_size_bitwidth_limit;
  }

  const JsonWrapper& get_json_config() const { return m_json; }

  /**
   * Get the global inliner config from the "inliner" section. If there is not
   * such section, will also look up "MethodInlinePass" section for backward
   * compatibility.
   */
  const inliner::InlinerConfig& get_inliner_config() {
    if (m_inliner_config == nullptr) {
      m_inliner_config = std::make_unique<inliner::InlinerConfig>();
      load_inliner_config(m_inliner_config.get());
    }
    return *m_inliner_config.get();
  }

  /**
   * Load configurations with the initial scope.
   */
  void load(const Scope& scope);

 private:
  JsonWrapper m_json;
  std::string outdir;

  std::vector<std::string> load_coldstart_classes();
  std::vector<std::string> load_coldstart_methods();
  std::unordered_map<std::string, std::vector<std::string> > load_class_lists();
  void load_method_to_weight();
  void load_method_sorting_whitelisted_substrings();
  void load_inliner_config(inliner::InlinerConfig*);

  bool m_move_map{false};
  bool m_load_class_lists_attempted{false};
  ProguardMap m_proguard_map;
  MethodMap m_moved_methods_map;
  std::string m_coldstart_class_filename;
  std::string m_coldstart_method_filename;
  std::string m_profiled_methods_filename;
  std::vector<std::string> m_coldstart_classes;
  std::vector<std::string> m_coldstart_methods;
  std::unordered_map<std::string, std::vector<std::string> > m_class_lists;
  std::unordered_map<std::string, unsigned int> m_method_to_weight;
  std::unordered_set<std::string> m_method_sorting_whitelisted_substrings;
  std::string m_printseeds; // Filename to dump computed seeds.

  // limits the output instruction size of any DexMethod to 2^n
  // 0 when limit is not present
  uint32_t m_instruction_size_bitwidth_limit;

  // global no optimizations annotations
  std::unordered_set<DexType*> m_no_optimizations_annos;
  // Global inliner config.
  std::unique_ptr<inliner::InlinerConfig> m_inliner_config{nullptr};
};
