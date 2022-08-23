#include "RunCommand.hpp"

#include <fmt/format.h>

#include <memory>

void RunOptions::setupRunOptions(CLI::App* parentApp) {
  /// Set up a subcommand and capture a shared_ptr to a struct that holds all its options.
  /// The variables of the struct are bound to the CLI options.
  /// We use a shared ptr so that the addresses of the variables remain for binding,
  /// You could return the shared pointer if you wanted to access the values in main.

  auto opt = std::make_shared<RunOptions>();

  auto* const app = parentApp->add_subcommand("run", "Executes an OpenStudio Workflow file");

  app->add_option("-w,--workflow", opt->osw_path, "Specify the FILE path to the workflow to run")->option_text("FILE");

  app->add_flag(
    "-m,--measures_only",
    [opt](std::int64_t val) {
      fmt::print("val={}\n", val);
      opt->no_simulation = (val == 1);
    },
    "Only run the OpenStudio and EnergyPlus measures");

  app->add_flag("-p,--postprocess_only", opt->post_process, "Only run the reporting measures");

  app->add_flag("--export-epJSON", opt->ep_json, "export epJSON file format. The default is IDF");

  app->add_option("-s,--socket", opt->socket_port, "Pipe status messages to a socket on localhost PORT")->option_text("PORT");

  auto* stdout_opt = app->add_flag("--show-stdout", opt->show_stdout, "export epJSON file format. The default is IDF")->group("Stdout Options");
  app->add_flag("--add-timings", opt->add_timings, "Print the start, end and elapsed times of each state of the simulation.")
    ->needs(stdout_opt)
    ->group("Stdout Options");
  app
    ->add_flag("--style-stdout", opt->style_stdout, "Style the stdout output to more clearly show the start and end of each state of the simulation.")
    ->needs(stdout_opt)
    ->group("Stdout Options");

  app->add_flag("--debug", opt->debug, "Includes additional outputs for debugging failing workflows and does not clean up the run directory");

  // FT options
  FtOptions::setupRunFtOptions(app, opt);

  app->callback([opt] { RunOptions::execute(*opt); });
}

void FtOptions::setupRunFtOptions(CLI::App* app, std::shared_ptr<RunOptions> opt) {
  app
    ->add_flag("--runcontrolspecialdays,!--no-runcontrolspecialdays", opt->ft_options.runcontrolspecialdays,
               "Include RunControlSpecialDays (Holidays)")
    ->group("Forward Translator Options");

  app->add_flag("--set-ip-tabular-output", opt->ft_options.ip_tabular_output, "Request IP units from E+ Tabular (HTML) Report [Default: False]")
    ->group("Forward Translator Options");

  app
    ->add_flag(
      "--lifecyclecosts,!--no-lifecyclecosts",
      [opt](std::int64_t val) {
        fmt::print("val={}\n", val);
        opt->ft_options.no_lifecyclecosts = (val != 1);
      },
      "Include LifeCycleCosts [Default: True]")
    ->group("Forward Translator Options");

  app
    ->add_flag(
      "--sqlite-output,!--no-sqlite-output",
      [opt](std::int64_t val) {
        fmt::print("val={}\n", val);
        opt->ft_options.no_sqlite_output = (val != 1);
      },
      "Request Output:SQLite from E+ [Default: True]")
    ->group("Forward Translator Options")
    ->default_val(false);

  app
    ->add_flag(
      "--html-output,!--no-html-output",
      [opt](std::int64_t val) {
        fmt::print("val={}\n", val);
        opt->ft_options.no_html_output = (val != 1);
      },
      "Request Output:Table:SummaryReports from E+ [Default: True]")
    ->group("Forward Translator Options")
    ->default_val(false);

  app
    ->add_flag(
      "--space-translation,!--no-space-translation",
      [opt](std::int64_t val) {
        fmt::print("val={}\n", val);
        opt->ft_options.no_space_translation = (val != 1);
      },
      "Add individual E+ Space [Default: True]")
    ->group("Forward Translator Options")
    ->default_val(false);
}

void RunOptions::execute(RunOptions const& opt) {
  fmt::print("\nRunOptions:\n");
  fmt::print("osw_path={}\n", opt.osw_path.string());
  fmt::print("debug={}\n", opt.debug);
  fmt::print("no_simulation={}\n", opt.no_simulation);
  fmt::print("post_process={}\n", opt.post_process);
  fmt::print("ep_json={}\n", opt.ep_json);
  fmt::print("show_stdout={}\n", opt.show_stdout);
  fmt::print("add_timings={}\n", opt.add_timings);
  fmt::print("style_stdout={}\n", opt.style_stdout);

  fmt::print("socket_port={}\n", opt.socket_port);

  opt.ft_options.debug_print();

  fmt::print("\n\n");
}

void FtOptions::debug_print() const {
  fmt::print("FtOptions:\n");
  fmt::print("  * runcontrolspecialdays={}\n", runcontrolspecialdays);
  fmt::print("  * ip_tabular_output={}\n", ip_tabular_output);
  fmt::print("  * no_lifecyclecosts={}\n", no_lifecyclecosts);
  fmt::print("  * no_sqlite_output={}\n", no_sqlite_output);
  fmt::print("  * no_html_output={}\n", no_html_output);
  fmt::print("  * no_space_translation={}\n", no_space_translation);
}
