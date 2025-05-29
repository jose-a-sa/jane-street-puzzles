#include <sys/types.h>
#include <tuple>

#include <fmt/ranges.h>
#include "spdlog/common.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "cmake-build-debug/_deps/spdlog-src/include/spdlog/sinks/rotating_file_sink.h"
#include "may-2025/number_cross_grid.h"

template<size_t N>
struct fmt::formatter<std::bitset<N>>
{
    constexpr auto parse(format_parse_context& ctx) const { return ctx.begin(); }

    template<typename FormatContext>
    auto format(std::bitset<N> const& b, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "{}", b.to_string());
    }
};

template<size_t N>
static void init_logging(char const (&log_file)[N])
{
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_st>(log_file, 20*1024*1024, 10000);
    file_sink->set_level(spdlog::level::debug);
    auto logger = std::make_shared<spdlog::logger>("", file_sink);
    spdlog::set_default_logger(logger);
    // spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%!] %v");
    spdlog::set_level(spdlog::level::debug);
    spdlog::flush_on(spdlog::level::info);
}


int main(int argc, char** argv)
{
    init_logging("number_cross_5.log");

    // clang-format off
    constexpr auto preds5 = std::make_tuple(
        is_multiple_of<11>{}, 
        is_multiple_of<14>{},
        is_multiple_of<28>{},
        is_multiple_of<101>{},
        is_multiple_of<2025>{}
    );

    auto H = [](uint8_t x) { return cell_info{x, true}; };
    
    constexpr auto islands5 = std::array<std::array<cell_info, 5>, 5>{{
        {H(0),H(0),  0 ,  0 ,  0 },
        {H(1),  0 ,  0 ,  0 ,  0 },
        {  1 ,  1 ,  0 ,  0 ,  0 },
        {  2 ,  1 ,  1 ,  0 ,H(0)},
        {  2 ,  2 ,  1 ,H(1),H(0)}
    }};
    
    constexpr auto preds11 = std::make_tuple(
        is_square{}, 
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


    constexpr auto islands11 = std::array<std::array<cell_info, 11>, 11>{{
        {  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        {  0 ,  1 ,  0 ,H(0),H(0),  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        {  1 ,  1 ,  3 ,  3 ,H(3),  3 ,  4 ,  4 ,  4 ,H(0),  4 },
        {  1 ,  3 ,  3 ,  1 ,  3 ,  5 ,  4 ,  4 ,H(4),H(4),  4 },
        {  1 ,  3 ,  3 ,  1 ,  3 ,  5 ,  5 ,  4 ,  4 ,  5 ,  4 },
        {  1 ,  1 ,  1 ,  1 ,  1 ,H(5),  5 ,  5 ,  5 ,  5 ,  4 },
        {  1 ,H(2),H(6),  6 ,  1 ,H(1),H(5),  5 ,  6 ,  5 ,  5 },
        {  1 ,H(2),  6 ,  6 ,  6 ,  6 ,  6 ,  6 ,  6 ,  7 ,  7 },
        {  2 ,  2 ,  2 ,  2 ,H(6),H(2),  6 ,  7 ,  7 ,  7 ,  7 },
        {  2 ,  2 ,  2 ,  2 ,H(2),  2 ,  2 ,  2 ,  2 ,  2 ,  2 },
        {  2 ,  2 ,  8 ,  8 ,  8 ,  8 ,  8 ,  8 ,  2 ,  2 ,  2 }
    }};

    // clang-format on

    // number_cross_grid grid5(preds5, islands5);
    //
    // number_cross_grid_solver solver5(grid5);
    // solver5.solve();
    // // solver5.solve_with_islands({5, 2, 8});
    //
    // fmt::println("\nGrid 5:\n{}", grid5);

    number_cross_grid grid11(preds11, islands11);

    spdlog::info("STATE:\n{}", grid11);

    number_cross_grid_solver solver11(grid11);
    solver11.solve();
    // solver11.solve_with_islands({1, 2, 5, 4, 9, 5, 8, 2, 6});

    fmt::println("\nGrid 11:\n{}", grid11);

    return 0;
}
