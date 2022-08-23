#pragma once

#include "Filesystem.hpp"
#include <CLI/App.hpp>

struct RunOptions;

struct FtOptions
{
 public:
  bool runcontrolspecialdays = true;
  bool ip_tabular_output = false;
  bool no_lifecyclecosts = false;
  bool no_sqlite_output = false;
  bool no_html_output = false;
  bool no_space_translation = false;

  static void setupRunFtOptions(CLI::App* app, std::shared_ptr<RunOptions> opt);

  void debug_print() const;
};

struct RunOptions
{
 public:
  static void setupRunOptions(CLI::App* parentApp);

  static void execute(RunOptions const& opt);

  bool debug = false;
  bool no_simulation = false;
  bool post_process = false;
  bool ep_json = false;
  bool show_stdout = false;
  bool add_timings = false;
  bool style_stdout = false;
  unsigned socket_port = 0;
  fs::path osw_path = "./workflow.osw";

  FtOptions ft_options;
};
