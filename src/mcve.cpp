#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <fmt/std.h>     // Formatting std::filesystem::path
#include <fmt/ranges.h>  // Formatting std::vector

#include <filesystem>

int main(int argc, char** argv) {

  CLI::App app("callback_passthrough");
  app.allow_extras(true);
  app.require_subcommand(1);

  auto* const experimentalApp = app.add_subcommand("labs");
  [[maybe_unused]] auto* energyplus_versionCommand =
    experimentalApp->add_subcommand("energyplus_version", "Returns the EnergyPlus version used by the CLI");
  app.callback([&app, &experimentalApp]() {
    fmt::print("app.remaining()={}\n", app.remaining());
    fmt::print("experimentalApp->remaining()={}\n", experimentalApp->remaining());

    fmt::print("app.remaining_for_passthrough()={}\n", app.remaining_for_passthrough());
    fmt::print("experimentalApp->remaining_for_passthrough()={}\n", experimentalApp->remaining_for_passthrough());

    // CLI::App subApp;
    // subApp.add_option("--" + argName, val, "custom argument option");
    // subApp.parse(app.remaining_for_passthrough());
  });

  CLI11_PARSE(app, argc, argv);
}
