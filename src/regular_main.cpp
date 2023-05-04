#include <CLI/CLI.hpp>

#include <fmt/format.h>
#include <fmt/ranges.h>  // vector formatting
#include <fmt/std.h>     // fs::path formatting

#include <algorithm>
#include <codecvt>
#include <iterator>
#include <locale>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

#define USE_CLI11 0

#if USE_CLI11

#  define USE_WMAIN 1

namespace detail {
inline std::string narrow_impl(const wchar_t* str, std::size_t str_size) {
#  ifdef _WIN32
  return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(str, str + str_size);
#  else
  return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str, str + str_size);
#  endif
}
}  // namespace detail

inline std::string narrow(const wchar_t* str) {
  return detail::narrow_impl(str, std::wcslen(str));
}

inline std::string narrow(const std::wstring& str) {
  return detail::narrow_impl(str.data(), str.size());
}

std::wstring widden(const std::string& str) {
#  ifdef _WIN32
  return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(str.data(), str.data() + str.size());
#  else
  return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str.data(), str.data() + str.size());
#  endif  // _WIN32
}

#  if (defined _WIN32) && USE_WMAIN

#    include <fmt/xchar.h>

std::string toString(const std::wstring& utf16_string) {
#    if _MSC_VER >= 1900
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  return convert.to_bytes(utf16_string.data(), utf16_string.data() + utf16_string.size());
#    else
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  const std::u16string u16string{utf16_string.begin(), utf16_string.end()};
  return convert.to_bytes(u16string);
#    endif
}

int wmain(int argc, wchar_t* wargv[]) {
  fmt::print("ori: argc={}\n", argc);
  for (int i = 0; i < argc; ++i) {
    // fmt::print("{}\n", wargv[i]);
  }

  static std::vector<std::string> args_as_strings;
  args_as_strings.reserve(static_cast<size_t>(argc));
  for (size_t i = 0; i < static_cast<size_t>(argc); ++i) {
    args_as_strings.push_back(narrow(wargv[i]));
  }
  fmt::print("args_as_strings={}\n", args_as_strings);

  static std::vector<const char*> static_args_result;
  static_args_result.reserve(static_cast<size_t>(argc));
  for (const auto& arg : args_as_strings) {
    static_args_result.push_back(arg.data());
  }

  std::vector<std::wstring> wargs(wargv, std::next(wargv, static_cast<std::ptrdiff_t>(argc)));
  std::vector<std::string> args;
  args.reserve(wargs.size());
  std::transform(wargs.cbegin(), wargs.cend(), std::back_inserter(args), [](const std::wstring& ws) { return toString(ws); });
  // fmt::print("wargs={}\n", wargs);
  fmt::print("args={}\n", args);

  fs::path p(args.back());
  fmt::print("{} - {}\n", p, fs::exists(p));
}
#  else

int main(int argc, const char* argv[]) {

  std::vector<std::string> cli11_args(CLI::argv(), std::next(CLI::argv(), static_cast<std::ptrdiff_t>(argc)));
  fmt::print("cli11_args={}\n", cli11_args);

  std::vector<std::string> args(argv, std::next(argv, static_cast<std::ptrdiff_t>(argc)));
  fmt::print("args={}\n", args);

  fs::path p(args.back());
  fmt::print("{} - {}\n", p, fs::exists(p));
}
#  endif
