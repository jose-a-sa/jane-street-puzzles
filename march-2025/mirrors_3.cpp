#include <memory>
#include <vector>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>

#include "march-2025/mirror_grid.h"


static void init_logging(char const* log_file)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    console_sink->set_level(spdlog::level::err);
    auto basic_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(log_file, true);
    basic_sink->set_level(spdlog::level::trace);
    auto logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{console_sink, basic_sink});
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::trace);
}


int main(int argc, char** argv)
{
    std::vector<uint64_t> left;
    std::vector<uint64_t> top;
    std::vector<uint64_t> right;
    std::vector<uint64_t> bottom;

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


    init_logging("mirrors_3.log");


    if(opt_left->count() && opt_top->count() && opt_right->count() && opt_bottom->count())
    {
        mirror_grid        grid(left, top, right, bottom);
        mirror_grid_solver solver(grid);
        if(solver.solve())
        {
            auto const res = grid.get_result();
            fmt::println("{}\nLeft: {}, Top: {}, Right: {}, Bottom: {}, Product: {}", grid, res.left, res.top,
                         res.right, res.bottom, res.product);
        }
        else
        {
            fmt::print("No solution found: {}", grid);
        }
    }
    else
    {
        mirror_grid grid5({0, 0, 0, 16, 0}, {0, 0, 9, 0, 0}, {0, 75, 0, 0, 0}, {0, 0, 36, 0, 0});

        mirror_grid_solver solver5(grid5);
        solver5.solve();

        auto const res5 = grid5.get_result();
        fmt::println("{}\nLeft: {}, Top: {}, Right: {}, Bottom: {}, Product: {}", grid5, res5.left, res5.top,
                     res5.right, res5.bottom, res5.product);

        mirror_grid grid10({0, 0, 0, 27, 0, 0, 0, 12, 225, 0}, {0, 0, 112, 0, 48, 3087, 9, 0, 0, 1},
                           {0, 4, 27, 0, 0, 0, 16, 0, 0, 0}, {2025, 0, 0, 12, 64, 5, 0, 405, 0, 0});

        mirror_grid_solver solver10(grid10);
        solver10.solve();

        auto const res10 = grid10.get_result();
        fmt::println("{}\nLeft: {}, Top: {}, Right: {}, Bottom: {}, Product: {}", grid10, res10.left, res10.top,
                     res10.right, res10.bottom, res10.product);
    }

    return 0;
}
