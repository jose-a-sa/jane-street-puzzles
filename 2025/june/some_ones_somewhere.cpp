#include <array>
#include <atomic>
#include <memory>
#include <ranges>
#include <span>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <fmt/ranges.h>
#include <fmt/std.h>

#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "2025/june/partridge_tiling.h"
#include "2025/june/partridge_tiling_solver.h"
#include "utils/thread_mapper.h"


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

constexpr auto config_1_1 = std::array<square_tile, 27>{
    {{kSky, 9, 0},      {kSky, 18, 0},     {kSky, 27, 0},     {kSky, 36, 0},    {kBrown, 37, 9},
     {kBrown, 29, 9},   {kMagenta, 24, 9}, {kMagenta, 19, 9}, {kCyan, 13, 9},   {kYellow, 38, 31},
     {kYellow, 38, 38}, {kBrown, 30, 37},  {kBrown, 30, 29},  {kBrown, 22, 37}, {kSky, 13, 29},
     {kYellow, 15, 38}, {kYellow, 8, 38},  {kBrown, 0, 37},   {kBlue, 0, 33},   {kBlue, 4, 33},
     {kMagenta, 8, 33}, {kSky, 0, 24},     {kSky, 0, 15},     {kBlue, 0, 11},   {kOrange, 4, 12}}};

constexpr auto config_1_2 = std::array<square_tile, 31>{
    {{kSky, 36, 0},     {kSky, 36, 9},     {kSky, 36, 18},   {kSky, 36, 27},   {kSky, 36, 36},    {kYellow, 29, 0},
     {kYellow, 29, 7},  {kYellow, 29, 14}, {kBrown, 28, 21}, {kBrown, 28, 29}, {kBrown, 28, 37},  {kMagenta, 24, 16},
     {kOrange, 25, 21}, {kSky, 19, 36},    {kSky, 19, 27},   {kSky, 10, 36},   {kMagenta, 0, 40}, {kMagenta, 5, 40},
     {kBrown, 0, 32},   {kCyan, 0, 26},    {kYellow, 0, 19}, {kYellow, 0, 12}, {kBlue, 0, 0},     {kBlue, 0, 4},
     {kBlue, 0, 8},     {kSky, 4, 0},      {kOrange, 4, 9},  {kBrown, 13, 0},  {kBrown, 13, 8}}};

constexpr auto config_1_3 = std::array<square_tile, 31>{
    {{kSky, 0, 0},      {kSky, 9, 0},       {kSky, 18, 0},      {kSky, 27, 0},   {kSky, 36, 0},    {kBrown, 0, 9},
     {kBrown, 8, 9},    {kYellow, 16, 9},   {kYellow, 38, 9},   {kSky, 0, 17},   {kYellow, 9, 17}, {kMagenta, 0, 26},
     {kBlue, 5, 26},    {kGreen, 5, 30},    {kSky, 0, 36},      {kCyan, 39, 16}, {kCyan, 39, 22},  {kYellow, 32, 16},
     {kOrange, 29, 20}, {kMagenta, 34, 23}, {kMagenta, 29, 23}, {kSky, 36, 28},  {kSky, 27, 28},   {kOrange, 24, 28},
     {kCyan, 21, 31},   {kCyan, 15, 31},    {kBlue, 41, 37},    {kBlue, 41, 41}, {kBrown, 33, 37}, {kBrown, 25, 37},
     {kBrown, 17, 37}}};

constexpr auto config_2_1 = std::array<square_tile, 29>{
    {{kSky, 0, 0},      {kSky, 0, 9},     {kSky, 36, 0},      {kSky, 36, 9},    {kSky, 36, 18},   {kSky, 27, 0},
     {kCyan, 0, 18},    {kCyan, 6, 18},   {kOrange, 9, 15},   {kYellow, 9, 8},  {kBrown, 9, 0},   {kMagenta, 17, 0},
     {kMagenta, 22, 0}, {kYellow, 20, 5}, {kOrange, 17, 5},   {kBlue, 16, 8},   {kYellow, 0, 31}, {kYellow, 0, 38},
     {kBrown, 7, 37},   {kBrown, 15, 37}, {kBrown, 23, 37},   {kBrown, 31, 37}, {kCyan, 39, 39},  {kCyan, 39, 33},
     {kBlue, 35, 33},   {kGreen, 33, 35}, {kMagenta, 28, 32}, {kSky, 19, 28},   {kBlue, 15, 33}}};

constexpr auto config_2_2 = std::array<square_tile, 28>{
    {{kSky, 36, 0},   {kSky, 36, 9},     {kBrown, 28, 0},   {kBrown, 28, 8},   {kBrown, 28, 16}, {kBrown, 20, 0},
     {kBrown, 20, 8}, {kYellow, 13, 0},  {kYellow, 6, 0},   {kSky, 11, 7},     {kMagenta, 6, 7}, {kMagenta, 6, 12},
     {kBlue, 2, 0},   {kSky, 36, 36},    {kYellow, 29, 31}, {kYellow, 29, 38}, {kBrown, 21, 37}, {kBlue, 25, 33},
     {kCyan, 15, 39}, {kOrange, 12, 42}, {kSky, 0, 36},     {kYellow, 0, 29},  {kYellow, 0, 22}, {kMagenta, 7, 31}}};

constexpr auto config_2_3 = std::array<square_tile, 27>{
    {{kSky, 0, 0},     {kSky, 9, 0},       {kSky, 18, 0},      {kSky, 27, 0},     {kSky, 36, 0},   {kYellow, 0, 17},
     {kYellow, 0, 24}, {kYellow, 0, 31},   {kYellow, 0, 38},   {kBlue, 0, 9},     {kBlue, 0, 13},  {kCyan, 7, 33},
     {kCyan, 7, 39},   {kBrown, 7, 25},    {kSky, 13, 36},     {kGreen, 20, 34},  {kCyan, 22, 39}, {kCyan, 28, 39},
     {kCyan, 34, 39},  {kMagenta, 40, 35}, {kMagenta, 40, 40}, {kYellow, 22, 32}, {kBrown, 37, 9}, {kBrown, 29, 9},
     {kOrange, 26, 9}, {kSky, 36, 17},     {kYellow, 29, 17}}};

constexpr auto config_3_1 = std::array<square_tile, 26>{
    {{kSky, 0, 0},      {kSky, 9, 0},     {kSky, 18, 0},     {kSky, 27, 0},      {kSky, 36, 0},      {kBrown, 7, 9},
     {kYellow, 15, 9},  {kYellow, 22, 9}, {kBrown, 37, 9},   {kBrown, 29, 9},    {kYellow, 0, 9},    {kYellow, 15, 16},
     {kOrange, 12, 17}, {kSky, 36, 17},   {kYellow, 29, 17}, {kMagenta, 40, 26}, {kMagenta, 40, 31}, {kBlue, 36, 26},
     {kGreen, 38, 34},  {kSky, 36, 36},   {kBrown, 28, 37},  {kBrown, 20, 37},   {kBlue, 16, 41},    {kBrown, 0, 37},
     {kSky, 0, 28},     {kCyan, 0, 22}}};

constexpr auto config_3_2 = std::array<square_tile, 24>{
    {{kSky, 0, 0},      {kSky, 0, 9},      {kSky, 0, 18},     {kSky, 9, 0},     {kSky, 18, 0},    {kSky, 0, 36},
     {kSky, 27, 0},     {kBlue, 0, 27},    {kMagenta, 0, 31}, {kGreen, 5, 34},  {kYellow, 9, 38}, {kYellow, 16, 38},
     {kBrown, 23, 37},  {kYellow, 9, 9},   {kYellow, 16, 9},  {kCyan, 9, 16},   {kCyan, 15, 16},  {kBrown, 21, 16},
     {kMagenta, 9, 22}, {kYellow, 14, 22}, {kBrown, 37, 37},  {kBrown, 37, 29}, {kBlue, 41, 25},  {kOrange, 34, 42}}};

constexpr auto config_3_3 = std::array<square_tile, 25>{
    {{kSky, 36, 0},     {kSky, 36, 9},     {kSky, 36, 18},     {kSky, 36, 27},     {kSky, 36, 36},
     {kSky, 27, 36},    {kSky, 18, 36},    {kYellow, 22, 0},   {kYellow, 29, 0},   {kBrown, 28, 7},
     {kBrown, 28, 15},  {kBrown, 28, 23},  {kMagenta, 31, 31}, {kMagenta, 26, 31}, {kMagenta, 21, 31},
     {kYellow, 21, 24}, {kBlue, 24, 20},   {kOrange, 25, 7},   {kCyan, 0, 0},      {kCyan, 0, 6},
     {kCyan, 0, 12},    {kMagenta, 0, 18}, {kBlue, 0, 23},     {kBrown, 6, 0},     {kBrown, 6, 8}}};

constexpr auto tiling_configs = std::array<std::span<square_tile const>, 9>{
    config_1_1, config_1_2, config_1_3, config_2_1, config_2_2, config_2_3, config_3_1, config_3_2, config_3_3};


class thread_id_formatter : public spdlog::custom_flag_formatter
{
public:
    void format(spdlog::details::log_msg const&, std::tm const&, spdlog::memory_buf_t& dest) override
    {
        fmt::format_to(std::back_inserter(dest), "{}", thread_mapper::get_this_thread_id());
    }

    std::unique_ptr<spdlog::custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<thread_id_formatter>();
    }
};


template<size_t N>
static void init_logging(char const (&log_file)[N])
{
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);
    file_sink->set_level(spdlog::level::info);
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);

    auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{file_sink});
    spdlog::set_default_logger(logger);

    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<thread_id_formatter>('t');
    formatter->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [thread %t] [%^%l%$] %v");
    logger->set_formatter(std::move(formatter));

    spdlog::set_level(spdlog::level::info);
}


int main(int argc, char** argv)
{
    thread_mapper::set_this_thread_id(0);

    init_logging("some_ones_somewhere.log");
    spdlog::info("Starting some_ones_somewhere. Initializing threads");

    constexpr auto kGridSide       = partridge_square_tiling<9>::kGridSide;
    constexpr auto kSideSequence   = partridge_square_tiling<9>::kSideSequence;
    constexpr auto kUnusedPosition = partridge_square_tiling<9>::kUnusedPosition;

    constexpr auto kNumPartridgeRows = 3ull;
    constexpr auto kNumPartridgeCols = 3ull;
    constexpr auto kNumLettersMax    = kGridSide * std::max(kNumPartridgeRows, kNumPartridgeCols);

    std::vector<std::thread> threads;
    threads.reserve(tiling_configs.size());

    std::array<std::pair<int, int>, 9> ones_positions;

    for(auto [idx, one_pos, cfg]: std::views::zip(std::views::iota(0u), ones_positions, tiling_configs))
    {
        threads.emplace_back(
            [&](auto const i, auto const r, auto const c)
            {
                auto const this_tid = thread_mapper::set_this_thread_id(i + 1);
                spdlog::info("Initialized thread {}", this_tid);

                partridge_square_tiling<9>        til(cfg);
                partridge_square_tiling_solver<9> solver(til);

                auto tiles_view = std::views::zip(kSideSequence, til.tile_positions()) |
                                  std::views::filter([&](auto const& p) { return std::get<1>(p) != kUnusedPosition; });

                spdlog::info("Start completing tiling ({},{}): {}", r, c, tiles_view);
                auto solutions = solver.find_all();

                auto solutions_with_size_view =
                    solutions | std::views::transform([&](auto&& s) { return std::views::zip(kSideSequence, s); });
                if(solutions.size() == 1)
                {
                    spdlog::info("Found a single solution for tiling ({},{}): {}", r, c,
                                 solutions_with_size_view.front());
                    one_pos = solutions[0].front();
                }
                else
                {
                    spdlog::info("Found multiple solutions for tiling ({},{}): {}", r, c, solutions_with_size_view);
                    one_pos = kUnusedPosition;
                }
            },
            idx, idx / kNumPartridgeCols, idx % kNumPartridgeCols);
    }

    for(auto& t: threads)
        t.join();
    spdlog::info("Found all solutions");

    using namespace std::literals;
    auto const letters = std::views::repeat("ABCDEFGHIJKLMNOPQRSTUVWXYZ"sv, (kNumLettersMax / 26) + 1) |
                         std::views::join | std::views::take(kNumLettersMax) | std::ranges::to<std::string>();
    auto const sol_view =
        std::views::zip(std::views::iota(0ull), ones_positions) |
        std::views::transform(
            [&](auto const& t)
            {
                auto const& [idx, pos] = t;
                auto const i           = idx / kNumPartridgeCols;
                auto const j           = idx % kNumPartridgeCols;
                return std::make_tuple(letters[i * kGridSide + pos.first], letters[j * kGridSide + pos.second]);
            });

    spdlog::info("The solution is: {}", sol_view);
    fmt::println("The solution is: {}", sol_view);

    return 0;
}
