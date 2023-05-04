#include <CLI/CLI.hpp>
#include <CLI/Error.hpp>
#include <CLI/Validators.hpp>
#include <CLI/Validators.hpp>
#include <fmt/format.h>
#include <fmt/std.h>     // Formatting std::filesystem::path
#include <fmt/ranges.h>  // Formatting std::vector

#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char** argv) {

  CLI::App app("callback_passthrough");
  fs::path dir;
  app.add_option("DIR", dir, "Directory")->check(CLI::ExistingDirectory);
  // app
  //   .add_option_function<std::string>(
  //     "DIR", [&dir](const std::string& s) { dir = CLI::to_path(s); }, "Directory")
  //   ->check(CLI::ExistingDirectory);

  CLI11_PARSE(app);

  fmt::print("{} - {}\n", dir, fs::exists(dir));
}
