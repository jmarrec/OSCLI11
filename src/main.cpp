#include "Filesystem.hpp"
#include "RunCommand.hpp"

#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <fmt/std.h>     // Formatting std::filesystem::path
#include <fmt/ranges.h>  // Formatting std::vector

#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  CLI::App app{"openstudio CLI"};
  app.allow_extras(true);
  app.require_subcommand(1);

  bool verbose = false;
  app.add_flag("--verbose", verbose, "Print the full log to STDOUT");

  std::vector<std::string> includeDirs;
  app.add_option("-I,--include", includeDirs, "Add additional directory to add to front of Ruby $LOAD_PATH (may be used more than once)")
    ->option_text("DIR");

  std::vector<std::string> executeCmds;
  app.add_option("-e,--execute", includeDirs, "Execute one line of script (may be used more than once). Returns after executing commands.")
    ->option_text("DIR");

  std::vector<std::string> gemPathDirs;
  app.add_option("--gem_path", gemPathDirs, "Add additional directory to add to front of GEM_PATH environment variable (may be used more than once)")
    ->option_text("DIR");

  std::string gemHomeDir;
  app.add_option("--gem_home", gemHomeDir, "Set GEM_HOME environment variable")->option_text("DIR");

  std::string gemFile;
  app.add_option("--bundle", gemFile, "Use bundler for GEMFILE'")->option_text("GEMFILE");

  auto* const experimentalApp = app.add_subcommand("labs");
  [[maybe_unused]] auto* energyplus_versionCommand =
    experimentalApp->add_subcommand("energyplus_version", "Returns the EnergyPlus version used by the CLI");
  {
    auto* execute_ruby_scriptCommand = experimentalApp->add_subcommand("execute_ruby_script", "Executes a ruby file");
    fs::path rubyScriptPath;
    execute_ruby_scriptCommand->add_option("path", rubyScriptPath, "Path to ruby file")->required(true)->check(CLI::ExistingFile);
    execute_ruby_scriptCommand->allow_extras(true);
    execute_ruby_scriptCommand->footer("Any additional arguments passed after the ruby file are forwarded");
    execute_ruby_scriptCommand->callback([&] {
      fmt::print("rubyScriptPath={}\n", rubyScriptPath);
      fmt::print("remaining()={}\n", execute_ruby_scriptCommand->remaining());
    });
  }

  [[maybe_unused]] auto* gem_listCommand = experimentalApp->add_subcommand("gem_list", "Lists the set gems available to openstudio");
  [[maybe_unused]] auto* list_commandsCommand = experimentalApp->add_subcommand("list_commands", "Lists the entire set of available commands");
  [[maybe_unused]] auto* measureCommand = experimentalApp->add_subcommand("measure", "Updates measures and compute arguments");
  [[maybe_unused]] auto* openstudio_versionCommand =
    experimentalApp->add_subcommand("openstudio_version", "Returns the OpenStudio version used by the CLI");
  [[maybe_unused]] auto* ruby_versionCommand = experimentalApp->add_subcommand("ruby_version", "Returns the Ruby version used by the CLI");
  // [[maybe_unused]] auto* runCommand = experimentalApp->add_subcommand("run", "Executes an OpenStudio Workflow file");

  RunOptions::setupRunOptions(experimentalApp);

  [[maybe_unused]] auto* updateCommand = experimentalApp->add_subcommand("update", "Updates OpenStudio Models to the current version");

  //std::filesystem::path scriptPath;
  //experimentalApp->add_option("path", scriptPath, "Path to ruby/python script")->expected(-1)->check(CLI::ExistingFile);
  //experimentalApp->allow_extras(true);
  //experimentalApp->positionals_at_end();
  app.callback([&app, &experimentalApp]() {
    fmt::print("app.remaining()={}\n", app.remaining());
    fmt::print("experimentalApp->remaining()={}\n", experimentalApp->remaining());

    fmt::print("app.remaining_for_passthrough()={}\n", app.remaining_for_passthrough());
    fmt::print("experimentalApp->remaining_for_passthrough()={}\n", experimentalApp->remaining_for_passthrough());
  });

  // CLI11_PARSE(app, argc, argv);
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return app.exit(e);
  }

  fmt::print("verbose={}\n", verbose);
  fmt::print("includeDirs={}\n", fmt::join(includeDirs, ","));

  fmt::print("gemPathDirs={}\n", fmt::join(gemPathDirs, ","));
  fmt::print("gemHomeDir={}\n", gemHomeDir);

  return 0;
}
