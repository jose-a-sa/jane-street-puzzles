#include <array>
#include <cstdint>
#include <numeric>
#include <ranges>
#include <span>
#include <sys/types.h>
#include <tuple>

#include <fmt/ranges.h>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


#include "may-2025/number_cross_grid.h"
#include "may-2025/number_cross_grid_predicates.h"
#include "may-2025/number_cross_grid_solver.h"


template<size_t N>
static void init_logging(char const (&log_file)[N])
{
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(log_file, true);
    file_sink->set_level(spdlog::level::info);
    auto logger = std::make_shared<spdlog::logger>("", file_sink);
    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%!] %v");
    spdlog::set_level(spdlog::level::info);
    // spdlog::flush_on(spdlog::level::info);
}


int main(int argc, char** argv)
{
    init_logging("number_cross_5_single.log");

    // clang-format off
    constexpr auto preds5 = std::make_tuple(
        is_multiple_of<11>{},
        is_multiple_of<14>{},
        is_multiple_of<28>{},
        is_multiple_of<101>{},
        is_multiple_of<2025>{}
    );

    constexpr auto regions5 = std::array<std::array<uint8_t, 5>, 5>{
        {{0, 0, 0, 0, 0},
         {1, 0, 0, 0, 0},
         {1, 1, 0, 0, 0},
         {2, 1, 1, 0, 0},
         {2, 2, 1, 1, 0}}};

    constexpr auto highlighted5 = std::array<std::array<bool, 5>, 5>{
        {{1, 1, 0, 0, 0},
         {1, 0, 0, 0, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 0, 0, 1},
         {0, 0, 0, 1, 1}}};
    // clang-format on


    number_cross_grid grid5(preds5, regions5, highlighted5);

    number_cross_grid_solver solver5(grid5);
    solver5.solve();

    fmt::println("\nGrid 5 with initial digits:\n{:R}", grid5);
    fmt::println("\nGrid 5 after placing tiles:\n{:D}", grid5);
    fmt::println("\nGrid 5 unique numbers: {}, Sum: {}", solver5.get_unique_numbers(),
                 std::ranges::fold_left(solver5.get_unique_numbers(), int64_t{}, std::plus<>{}));


    // clang-format off
    constexpr auto preds11 = std::make_tuple(
        is_perfect_square{},
        product_of_digits_matches<20>{},
        is_multiple_of<13>{},
        is_multiple_of<32>{},
        is_divisible_by_its_digits{},
        product_of_digits_matches<25>{},
        is_divisible_by_its_digits{},
        is_odd_palindrome{},
        is_fibonacci{},
        product_of_digits_matches<2025>{},
        is_prime{}
    );

    constexpr auto regions11 = std::array<std::array<uint8_t, 11>, 11>{
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         {1, 1, 3, 3, 3, 3, 4, 4, 4, 0, 4},
         {1, 3, 3, 1, 3, 5, 4, 4, 4, 4, 4},
         {1, 3, 3, 1, 3, 5, 5, 4, 4, 5, 4},
         {1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 4},
         {1, 2, 6, 6, 1, 1, 5, 5, 6, 5, 5},
         {1, 2, 6, 6, 6, 6, 6, 6, 6, 7, 7},
         {2, 2, 2, 2, 6, 2, 6, 7, 7, 7, 7},
         {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
         {2, 2, 8, 8, 8, 8, 8, 8, 2, 2, 2}}};

    constexpr auto highlighted11 = std::array<std::array<bool, 11>, 11>{
        {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
         {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
         {0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0},
         {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};

    constexpr auto grid_region_digits_hint11 = std::array<uint8_t, 9>{{2, 4, 3, 3, 4, 1, 6, 7, 7}};
    // clang-format on

    number_cross_grid grid11(preds11, regions11, highlighted11);

    // number_cross_grid        grid11_hint = grid11;
    // number_cross_grid_solver solver11_hint(grid11_hint);
    // solver11_hint.solve_with_region_digits(grid_region_digits_hint11);

    // fmt::println("\nGrid 11 (hint) with initial digits:\n{:R}", grid11_hint);
    // fmt::println("\nGrid 11 (hint) after placing tiles:\n{:D}", grid11_hint);
    // fmt::println("\nGrid 11 (hint) unique numbers: {}, Sum: {}", solver11_hint.get_unique_numbers(),
    //              std::ranges::fold_left(solver11_hint.get_unique_numbers(), int64_t{}, std::plus<>{}));

    number_cross_grid_solver solver11(grid11);
    solver11.solve();

    fmt::println("\nGrid 11 with initial digits:\n{:R}", grid11);
    fmt::println("\nGrid 11 after placing tiles:\n{:D}", grid11);
    fmt::println("\nGrid 11 unique numbers: {}, Sum: {}", solver11.get_unique_numbers(),
                 std::ranges::fold_left(solver11.get_unique_numbers(), int64_t{}, std::plus<>{}));

    return 0;
}
