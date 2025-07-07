#include <array>
#include <span>

#include <fmt/ranges.h>
#include <fmt/std.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


#include "june-2025/partridge_tiling.h"
#include "june-2025/partridge_tiling_solver.h"
#include "spdlog/common.h"


template<size_t N>
static void init_logging(char const (&log_file)[N])
{
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);
    file_sink->set_level(spdlog::level::info);
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{file_sink, console_sink});
    spdlog::set_default_logger(logger);
    // spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%!] %v");
    spdlog::set_level(spdlog::level::info);
}

enum tile_color
{
    kRed     = 1,
    kGreen   = 2,
    kOrange  = 3,
    kBlue    = 4,
    kMagenta = 5,
    kCyan    = 6,
    kYellow  = 7,
    kBrown   = 8,
    kSky     = 9
};

int main(int argc, char** argv)
{
    init_logging("some_ones_somewhere.log");

    constexpr auto tilling1 = std::array<square_tile, 27>{
        {{kSky, 9, 0},      {kSky, 18, 0},     {kSky, 27, 0},     {kSky, 36, 0},    {kBrown, 37, 9},
         {kBrown, 29, 9},   {kMagenta, 24, 9}, {kMagenta, 19, 9}, {kCyan, 13, 9},   {kYellow, 38, 31},
         {kYellow, 38, 38}, {kBrown, 30, 37},  {kBrown, 30, 29},  {kBrown, 22, 37}, {kSky, 13, 29},
         {kYellow, 15, 38}, {kYellow, 8, 38},  {kBrown, 0, 37},   {kBlue, 0, 33},   {kBlue, 4, 33},
         {kMagenta, 8, 33}, {kSky, 0, 24},     {kSky, 0, 15},     {kBlue, 0, 11},   {kOrange, 4, 12}}};

    constexpr auto tilling2 = std::array<square_tile, 31>{
        {{kSky, 36, 0},    {kSky, 36, 9},      {kSky, 36, 18},    {kSky, 36, 27},   {kSky, 36, 36},
         {kYellow, 29, 0}, {kYellow, 29, 7},   {kYellow, 29, 14}, {kBrown, 28, 21}, {kBrown, 28, 29},
         {kBrown, 28, 37}, {kMagenta, 24, 16}, {kOrange, 25, 21}, {kSky, 19, 36},   {kSky, 19, 27},
         {kSky, 10, 36},   {kMagenta, 0, 40},  {kMagenta, 5, 40}, {kBrown, 0, 32},  {kCyan, 0, 26},
         {kYellow, 0, 19}, {kYellow, 0, 12},   {kBlue, 0, 0},     {kBlue, 0, 4},    {kBlue, 0, 8},
         {kSky, 4, 0},     {kOrange, 4, 9},    {kBrown, 13, 0},   {kBrown, 13, 8}}};

    constexpr auto tilling3 = std::array<square_tile, 31>{
        {{kSky, 0, 0},       {kSky, 9, 0},      {kSky, 18, 0},     {kSky, 27, 0},     {kSky, 36, 0},
         {kBrown, 0, 9},     {kBrown, 8, 9},    {kYellow, 16, 9},  {kYellow, 38, 9},  {kSky, 0, 17},
         {kYellow, 9, 17},   {kMagenta, 0, 26}, {kBlue, 5, 26},    {kGreen, 5, 30},   {kSky, 0, 36},
         {kCyan, 39, 16},    {kCyan, 39, 22},   {kYellow, 32, 16}, {kOrange, 29, 20}, {kMagenta, 34, 23},
         {kMagenta, 29, 23}, {kSky, 36, 28},    {kSky, 27, 28},    {kOrange, 24, 28}, {kCyan, 21, 31},
         {kCyan, 15, 31},    {kBlue, 41, 37},   {kBlue, 41, 41},   {kBrown, 33, 37},  {kBrown, 25, 37},
         {kBrown, 17, 37}}};

    constexpr auto tilling4 = std::array<square_tile, 29>{
        {{kSky, 0, 0},      {kSky, 0, 9},     {kSky, 36, 0},      {kSky, 36, 9},    {kSky, 36, 18},   {kSky, 27, 0},
         {kCyan, 0, 18},    {kCyan, 6, 18},   {kOrange, 9, 15},   {kYellow, 9, 8},  {kBrown, 9, 0},   {kMagenta, 17, 0},
         {kMagenta, 22, 0}, {kYellow, 20, 5}, {kOrange, 17, 5},   {kBlue, 16, 8},   {kYellow, 0, 31}, {kYellow, 0, 38},
         {kBrown, 7, 37},   {kBrown, 15, 37}, {kBrown, 23, 37},   {kBrown, 31, 37}, {kCyan, 39, 39},  {kCyan, 39, 33},
         {kBlue, 35, 33},   {kGreen, 33, 35}, {kMagenta, 28, 32}, {kSky, 19, 28},   {kBlue, 15, 33}}};

    constexpr auto tilling5 = std::array<square_tile, 29>{
        {{kSky, 36, 0},     {kSky, 36, 9},     {kBrown, 28, 0},  {kBrown, 28, 8},  {kBrown, 28, 16},
         {kBrown, 20, 0},   {kBrown, 20, 8},   {kYellow, 13, 0}, {kYellow, 6, 0},  {kSky, 11, 7},
         {kMagenta, 6, 7},  {kMagenta, 6, 12}, {kBlue, 2, 0},    {kSky, 36, 36},   {kYellow, 29, 31},
         {kYellow, 29, 38}, {kBrown, 21, 37},  {kBlue, 25, 33},  {kCyan, 15, 39},  {kOrange, 12, 42},
         {kSky, 0, 36},     {kYellow, 0, 29},  {kYellow, 0, 22}, {kMagenta, 7, 31}}};

    constexpr auto tilling6 = std::array<square_tile, 27>{
        {{kSky, 0, 0},     {kSky, 9, 0},       {kSky, 18, 0},      {kSky, 27, 0},     {kSky, 36, 0},   {kYellow, 0, 17},
         {kYellow, 0, 24}, {kYellow, 0, 31},   {kYellow, 0, 38},   {kBlue, 0, 9},     {kBlue, 0, 13},  {kCyan, 7, 33},
         {kCyan, 7, 39},   {kBrown, 7, 25},    {kSky, 13, 36},     {kGreen, 20, 34},  {kCyan, 22, 39}, {kCyan, 28, 39},
         {kCyan, 34, 39},  {kMagenta, 40, 35}, {kMagenta, 40, 40}, {kYellow, 22, 32}, {kBrown, 37, 9}, {kBrown, 29, 9},
         {kOrange, 26, 9}, {kSky, 36, 17},     {kYellow, 29, 17}}};

    constexpr auto tilling7 = std::array<square_tile, 26>{
        {{kSky, 0, 0},       {kSky, 9, 0},       {kSky, 18, 0},     {kSky, 27, 0},    {kSky, 36, 0},
         {kBrown, 7, 9},     {kYellow, 15, 9},   {kYellow, 22, 9},  {kBrown, 37, 9},  {kBrown, 29, 9},
         {kYellow, 0, 9},    {kYellow, 15, 16},  {kOrange, 12, 17}, {kSky, 36, 17},   {kYellow, 29, 17},
         {kMagenta, 40, 26}, {kMagenta, 40, 31}, {kBlue, 36, 26},   {kGreen, 38, 34}, {kSky, 36, 36},
         {kBrown, 28, 37},   {kBrown, 20, 37},   {kBlue, 16, 41},   {kBrown, 0, 37},  {kSky, 0, 28},
         {kCyan, 0, 22}}};

    constexpr auto tilling8 = std::array<square_tile, 24>{
        {{kSky, 0, 0},     {kSky, 0, 9},      {kSky, 0, 18},    {kSky, 9, 0},      {kSky, 18, 0},
         {kSky, 0, 36},    {kSky, 27, 0},     {kBlue, 0, 27},   {kMagenta, 0, 31}, {kGreen, 5, 34},
         {kYellow, 9, 38}, {kYellow, 16, 38}, {kBrown, 23, 37}, {kYellow, 9, 9},   {kYellow, 16, 9},
         {kCyan, 9, 16},   {kCyan, 15, 16},   {kBrown, 21, 16}, {kMagenta, 9, 22}, {kYellow, 14, 22},
         {kBrown, 37, 37}, {kBrown, 37, 29},  {kBlue, 41, 25},  {kOrange, 34, 42}}};

    constexpr auto tilling9 = std::array<square_tile, 25>{
        {{kSky, 36, 0},     {kSky, 36, 9},     {kSky, 36, 18},     {kSky, 36, 27},     {kSky, 36, 36},
         {kSky, 27, 36},    {kSky, 18, 36},    {kYellow, 22, 0},   {kYellow, 29, 0},   {kBrown, 28, 7},
         {kBrown, 28, 15},  {kBrown, 28, 23},  {kMagenta, 31, 31}, {kMagenta, 26, 31}, {kMagenta, 21, 31},
         {kYellow, 21, 24}, {kBlue, 24, 20},   {kOrange, 25, 7},   {kCyan, 0, 0},      {kCyan, 0, 6},
         {kCyan, 0, 12},    {kMagenta, 0, 18}, {kBlue, 0, 23},     {kBrown, 6, 0},     {kBrown, 6, 8}}};


    partridge_square_tiling<9> til(tilling3);

    fmt::println("Tiling with counts: {}\n{}", til.tile_counts(), til);

    partridge_square_tiling_solver_reversed solver(til);
    auto const                              sols = solver.solve();

    spdlog::info("Found {} solutions", sols.size());
    for(auto const& sol: sols)
    {
        spdlog::info("SOL: {}", sol);
    }

    return 0;
}
