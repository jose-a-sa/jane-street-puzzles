#include <initializer_list>
#include <memory>
#include <print>
#include <vector>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>

#include "2025/march/mirror_grid.h"
#include "2025/march/mirror_grid_solver.h"
#include "spdlog/common.h"


static void init_logging(char const* log_file)
{
    // auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    // console_sink->set_level(spdlog::level::info);
    auto basic_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(log_file, true);
    basic_sink->set_level(spdlog::level::debug);
    auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{basic_sink});
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
}


int main(int argc, char** argv)
{
    init_logging("mirrors_3.log");
    spdlog::info("Starting mirrors-3.");

    std::vector<uint32_t> left;
    std::vector<uint32_t> top;
    std::vector<uint32_t> right;
    std::vector<uint32_t> bottom;

    CLI::App app{"Hall of mirrors 3 solver"};
    argv            = app.ensure_utf8(argv);
    auto opt_left   = app.add_option("-l,--left", left, "Left numbers of the grid")->delimiter(',');
    auto opt_top    = app.add_option("-t,--top", top, "Top numbers of the grid")->delimiter(',');
    auto opt_right  = app.add_option("-r,--right", right, "Right numbers of the grid")->delimiter(',');
    auto opt_bottom = app.add_option("-b,--bottom", bottom, "Bottom numbers of the grid")->delimiter(',');
    app.callback(
        [&]
        {
            if(left.size() != top.size() || top.size() != right.size() || right.size() != bottom.size())
                throw CLI::ValidationError("Error: all sides of the grid must have the same length.");
        });
    CLI11_PARSE(app, argc, argv);

    auto const solve_and_print = []<class... Lists>(Lists&&... ls)
    {
        mirror_grid grid(std::forward<Lists>(ls)...);
        auto const  n = grid.length();
        fmt::println("Grid ({}*{}): {}", n, n, grid);
        spdlog::info("Starting solving grid ({}*{})", n, n);

        mirror_grid_solver solver(grid);
        bool const         is_solved = solver.solve();
        spdlog::info("Finished grid ({}*{}). Solved={}", n, n, is_solved);

        if(is_solved)
        {
            auto const res = grid.compute_result();
            fmt::println("Left: {}, Top: {}, Right: {}, Bottom: {}, Product: {}", res.left, res.top, res.right,
                         res.bottom, res.product);
            fmt::println("Solved grid ({}*{}): {}", n, n, grid);
        }
        else
            fmt::println("No solution found for grid ({}*{}): {}", n, n, grid);
    };


    if(opt_left->count() & opt_top->count() & opt_right->count() & opt_bottom->count())
    {
        solve_and_print(left, top, right, bottom);
    }
    else
    {
        using UL = std::initializer_list<uint32_t>;

        solve_and_print(UL{0, 0, 0, 16, 0}, UL{0, 0, 9, 0, 0}, UL{0, 75, 0, 0, 0}, UL{0, 0, 36, 0, 0});

        solve_and_print(UL{0, 0, 0, 27, 0, 0, 0, 12, 225, 0}, UL{0, 0, 112, 0, 48, 3087, 9, 0, 0, 1},
                        UL{0, 4, 27, 0, 0, 0, 16, 0, 0, 0}, UL{2025, 0, 0, 12, 64, 5, 0, 405, 0, 0});
    }

    return 0;
}
