
#include <vector>

#include "mirror_grid.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


int main()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::err);

    auto basic_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("mirror_3.log", true);
    basic_sink->set_level(spdlog::level::trace);

    std::vector<spdlog::sink_ptr> sinks{console_sink, basic_sink};
    auto                          logger = std::make_shared<spdlog::logger>("", sinks.begin(), sinks.end());
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::trace);

    mirror_grid        grid5({0, 0, 0, 16, 0}, {0, 0, 9, 0, 0}, {0, 75, 0, 0, 0}, {0, 0, 36, 0, 0});
    mirror_grid_solver solver5{grid5};
    solver5.solve();

    fmt::println("{}", grid5);

    auto const res5 = grid5.get_missing_sums();
    fmt::println("Left: {}, Top: {}, Right: {}, Bottom: {}, Product: {}", res5.left, res5.top, res5.right, res5.bottom,
                 res5.product);

    mirror_grid        grid10({0, 0, 0, 27, 0, 0, 0, 12, 225, 0}, {0, 0, 112, 0, 48, 3087, 9, 0, 0, 1},
                              {0, 4, 27, 0, 0, 0, 16, 0, 0, 0}, {2025, 0, 0, 12, 64, 5, 0, 405, 0, 0});
    mirror_grid_solver solver10{grid10};
    solver10.solve();

    fmt::println("{}", grid10);

    auto const res10 = grid10.get_missing_sums();
    fmt::println("Left: {}, Top: {}, Right: {}, Bottom: {}, Product: {}", res10.left, res10.top, res10.right,
                 res10.bottom, res10.product);

    return 0;
}
