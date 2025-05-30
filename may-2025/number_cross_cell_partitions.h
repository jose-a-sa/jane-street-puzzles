#ifndef NUMBER_CROSS_CELL_PARTITIONS_H
#define NUMBER_CROSS_CELL_PARTITIONS_H

#include <array>
#include <iterator>
#include <span>

struct cell_partition
{
    uint8_t left;
    uint8_t top;
    uint8_t bottom;
    uint8_t right;
};

struct cell_partitions
{
    static constexpr auto& get(int number) { return cell_partitions_[number]; }

private:
    // clang-format off
    static constexpr auto zero_partitions_ = std::array<cell_partition, 0>{};
    static constexpr auto one_partitions_ = std::array<cell_partition, 4>{
        {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
    static constexpr auto two_partitions_ = std::array<cell_partition, 10>{
        {{2, 0, 0, 0}, {0, 2, 0, 0}, {0, 0, 2, 0}, {0, 0, 0, 2}, {1, 1, 0, 0},
         {1, 0, 1, 0}, {1, 0, 0, 1}, {0, 1, 1, 0}, {0, 1, 0, 1}, {0, 0, 1, 1}}};
    static constexpr auto three_partitions_ = std::array<cell_partition, 20>{
        {{3, 0, 0, 0}, {0, 3, 0, 0}, {0, 0, 3, 0}, {0, 0, 0, 3}, {2, 1, 0, 0}, {2, 0, 1, 0}, {2, 0, 0, 1},
         {1, 2, 0, 0}, {1, 0, 2, 0}, {1, 0, 0, 2}, {0, 2, 1, 0}, {0, 2, 0, 1}, {0, 1, 2, 0}, {0, 1, 0, 2},
         {0, 0, 2, 1}, {0, 0, 1, 2}, {1, 1, 1, 0}, {1, 1, 0, 1}, {1, 0, 1, 1}, {0, 1, 1, 1}}};
    static constexpr auto four_partitions_ = std::array<cell_partition, 35>{
        {{4, 0, 0, 0}, {0, 4, 0, 0}, {0, 0, 4, 0}, {0, 0, 0, 4}, {3, 1, 0, 0}, {3, 0, 1, 0}, {3, 0, 0, 1},
         {1, 3, 0, 0}, {1, 0, 3, 0}, {1, 0, 0, 3}, {0, 3, 1, 0}, {0, 3, 0, 1}, {0, 1, 3, 0}, {0, 1, 0, 3},
         {0, 0, 3, 1}, {0, 0, 1, 3}, {2, 2, 0, 0}, {2, 0, 2, 0}, {2, 0, 0, 2}, {0, 2, 2, 0}, {0, 2, 0, 2},
         {0, 0, 2, 2}, {2, 1, 1, 0}, {2, 1, 0, 1}, {2, 0, 1, 1}, {1, 2, 1, 0}, {1, 2, 0, 1}, {1, 1, 2, 0},
         {1, 1, 0, 2}, {1, 0, 2, 1}, {1, 0, 1, 2}, {0, 2, 1, 1}, {0, 1, 2, 1}, {0, 1, 1, 2}, {1, 1, 1, 1}}};
    static constexpr auto five_partitions_ = std::array<cell_partition, 56>{
        {{5, 0, 0, 0}, {0, 5, 0, 0}, {0, 0, 5, 0}, {0, 0, 0, 5}, {4, 1, 0, 0}, {4, 0, 1, 0}, {4, 0, 0, 1},
         {1, 4, 0, 0}, {1, 0, 4, 0}, {1, 0, 0, 4}, {0, 4, 1, 0}, {0, 4, 0, 1}, {0, 1, 4, 0}, {0, 1, 0, 4},
         {0, 0, 4, 1}, {0, 0, 1, 4}, {3, 2, 0, 0}, {3, 0, 2, 0}, {3, 0, 0, 2}, {2, 3, 0, 0}, {2, 0, 3, 0},
         {2, 0, 0, 3}, {0, 3, 2, 0}, {0, 3, 0, 2}, {0, 2, 3, 0}, {0, 2, 0, 3}, {0, 0, 3, 2}, {0, 0, 2, 3},
         {3, 1, 1, 0}, {3, 1, 0, 1}, {3, 0, 1, 1}, {1, 3, 1, 0}, {1, 3, 0, 1}, {1, 1, 3, 0}, {1, 1, 0, 3},
         {1, 0, 3, 1}, {1, 0, 1, 3}, {0, 3, 1, 1}, {0, 1, 3, 1}, {0, 1, 1, 3}, {2, 2, 1, 0}, {2, 2, 0, 1},
         {2, 1, 2, 0}, {2, 1, 0, 2}, {2, 0, 2, 1}, {2, 0, 1, 2}, {1, 2, 2, 0}, {1, 2, 0, 2}, {1, 0, 2, 2},
         {0, 2, 2, 1}, {0, 2, 1, 2}, {0, 1, 2, 2}, {2, 1, 1, 1}, {1, 2, 1, 1}, {1, 1, 2, 1}, {1, 1, 1, 2}}};
    static constexpr auto six_partitions_ = std::array<cell_partition, 84>{
        {{6, 0, 0, 0}, {0, 6, 0, 0}, {0, 0, 6, 0}, {0, 0, 0, 6}, {5, 1, 0, 0}, {5, 0, 1, 0}, {5, 0, 0, 1}, {1, 5, 0, 0},
         {1, 0, 5, 0}, {1, 0, 0, 5}, {0, 5, 1, 0}, {0, 5, 0, 1}, {0, 1, 5, 0}, {0, 1, 0, 5}, {0, 0, 5, 1}, {0, 0, 1, 5},
         {4, 2, 0, 0}, {4, 0, 2, 0}, {4, 0, 0, 2}, {2, 4, 0, 0}, {2, 0, 4, 0}, {2, 0, 0, 4}, {0, 4, 2, 0}, {0, 4, 0, 2},
         {0, 2, 4, 0}, {0, 2, 0, 4}, {0, 0, 4, 2}, {0, 0, 2, 4}, {4, 1, 1, 0}, {4, 1, 0, 1}, {4, 0, 1, 1}, {1, 4, 1, 0},
         {1, 4, 0, 1}, {1, 1, 4, 0}, {1, 1, 0, 4}, {1, 0, 4, 1}, {1, 0, 1, 4}, {0, 4, 1, 1}, {0, 1, 4, 1}, {0, 1, 1, 4},
         {3, 3, 0, 0}, {3, 0, 3, 0}, {3, 0, 0, 3}, {0, 3, 3, 0}, {0, 3, 0, 3}, {0, 0, 3, 3}, {3, 2, 1, 0}, {3, 2, 0, 1},
         {3, 1, 2, 0}, {3, 1, 0, 2}, {3, 0, 2, 1}, {3, 0, 1, 2}, {2, 3, 1, 0}, {2, 3, 0, 1}, {2, 1, 3, 0}, {2, 1, 0, 3},
         {2, 0, 3, 1}, {2, 0, 1, 3}, {1, 3, 2, 0}, {1, 3, 0, 2}, {1, 2, 3, 0}, {1, 2, 0, 3}, {1, 0, 3, 2}, {1, 0, 2, 3},
         {0, 3, 2, 1}, {0, 3, 1, 2}, {0, 2, 3, 1}, {0, 2, 1, 3}, {0, 1, 3, 2}, {0, 1, 2, 3}, {3, 1, 1, 1}, {1, 3, 1, 1},
         {1, 1, 3, 1}, {1, 1, 1, 3}, {2, 2, 2, 0}, {2, 2, 0, 2}, {2, 0, 2, 2}, {0, 2, 2, 2}, {2, 2, 1, 1}, {2, 1, 2, 1},
         {2, 1, 1, 2}, {1, 2, 2, 1}, {1, 2, 1, 2}, {1, 1, 2, 2}}};
    static constexpr auto seven_partitions_ = std::array<cell_partition, 120>{
        {{7, 0, 0, 0}, {0, 7, 0, 0}, {0, 0, 7, 0}, {0, 0, 0, 7}, {6, 1, 0, 0}, {6, 0, 1, 0}, {6, 0, 0, 1},
         {1, 6, 0, 0}, {1, 0, 6, 0}, {1, 0, 0, 6}, {0, 6, 1, 0}, {0, 6, 0, 1}, {0, 1, 6, 0}, {0, 1, 0, 6},
         {0, 0, 6, 1}, {0, 0, 1, 6}, {5, 2, 0, 0}, {5, 0, 2, 0}, {5, 0, 0, 2}, {2, 5, 0, 0}, {2, 0, 5, 0},
         {2, 0, 0, 5}, {0, 5, 2, 0}, {0, 5, 0, 2}, {0, 2, 5, 0}, {0, 2, 0, 5}, {0, 0, 5, 2}, {0, 0, 2, 5},
         {5, 1, 1, 0}, {5, 1, 0, 1}, {5, 0, 1, 1}, {1, 5, 1, 0}, {1, 5, 0, 1}, {1, 1, 5, 0}, {1, 1, 0, 5},
         {1, 0, 5, 1}, {1, 0, 1, 5}, {0, 5, 1, 1}, {0, 1, 5, 1}, {0, 1, 1, 5}, {4, 3, 0, 0}, {4, 0, 3, 0},
         {4, 0, 0, 3}, {3, 4, 0, 0}, {3, 0, 4, 0}, {3, 0, 0, 4}, {0, 4, 3, 0}, {0, 4, 0, 3}, {0, 3, 4, 0},
         {0, 3, 0, 4}, {0, 0, 4, 3}, {0, 0, 3, 4}, {4, 2, 1, 0}, {4, 2, 0, 1}, {4, 1, 2, 0}, {4, 1, 0, 2},
         {4, 0, 2, 1}, {4, 0, 1, 2}, {2, 4, 1, 0}, {2, 4, 0, 1}, {2, 1, 4, 0}, {2, 1, 0, 4}, {2, 0, 4, 1},
         {2, 0, 1, 4}, {1, 4, 2, 0}, {1, 4, 0, 2}, {1, 2, 4, 0}, {1, 2, 0, 4}, {1, 0, 4, 2}, {1, 0, 2, 4},
         {0, 4, 2, 1}, {0, 4, 1, 2}, {0, 2, 4, 1}, {0, 2, 1, 4}, {0, 1, 4, 2}, {0, 1, 2, 4}, {4, 1, 1, 1},
         {1, 4, 1, 1}, {1, 1, 4, 1}, {1, 1, 1, 4}, {3, 3, 1, 0}, {3, 3, 0, 1}, {3, 1, 3, 0}, {3, 1, 0, 3},
         {3, 0, 3, 1}, {3, 0, 1, 3}, {1, 3, 3, 0}, {1, 3, 0, 3}, {1, 0, 3, 3}, {0, 3, 3, 1}, {0, 3, 1, 3},
         {0, 1, 3, 3}, {3, 2, 2, 0}, {3, 2, 0, 2}, {3, 0, 2, 2}, {2, 3, 2, 0}, {2, 3, 0, 2}, {2, 2, 3, 0},
         {2, 2, 0, 3}, {2, 0, 3, 2}, {2, 0, 2, 3}, {0, 3, 2, 2}, {0, 2, 3, 2}, {0, 2, 2, 3}, {3, 2, 1, 1},
         {3, 1, 2, 1}, {3, 1, 1, 2}, {2, 3, 1, 1}, {2, 1, 3, 1}, {2, 1, 1, 3}, {1, 3, 2, 1}, {1, 3, 1, 2},
         {1, 2, 3, 1}, {1, 2, 1, 3}, {1, 1, 3, 2}, {1, 1, 2, 3}, {2, 2, 2, 1}, {2, 2, 1, 2}, {2, 1, 2, 2},
         {1, 2, 2, 2}}};
    static constexpr auto eight_partitions_ = std::array<cell_partition, 165>{
        {{8, 0, 0, 0}, {0, 8, 0, 0}, {0, 0, 8, 0}, {0, 0, 0, 8}, {7, 1, 0, 0}, {7, 0, 1, 0}, {7, 0, 0, 1}, {1, 7, 0, 0},
         {1, 0, 7, 0}, {1, 0, 0, 7}, {0, 7, 1, 0}, {0, 7, 0, 1}, {0, 1, 7, 0}, {0, 1, 0, 7}, {0, 0, 7, 1}, {0, 0, 1, 7},
         {6, 2, 0, 0}, {6, 0, 2, 0}, {6, 0, 0, 2}, {2, 6, 0, 0}, {2, 0, 6, 0}, {2, 0, 0, 6}, {0, 6, 2, 0}, {0, 6, 0, 2},
         {0, 2, 6, 0}, {0, 2, 0, 6}, {0, 0, 6, 2}, {0, 0, 2, 6}, {6, 1, 1, 0}, {6, 1, 0, 1}, {6, 0, 1, 1}, {1, 6, 1, 0},
         {1, 6, 0, 1}, {1, 1, 6, 0}, {1, 1, 0, 6}, {1, 0, 6, 1}, {1, 0, 1, 6}, {0, 6, 1, 1}, {0, 1, 6, 1}, {0, 1, 1, 6},
         {5, 3, 0, 0}, {5, 0, 3, 0}, {5, 0, 0, 3}, {3, 5, 0, 0}, {3, 0, 5, 0}, {3, 0, 0, 5}, {0, 5, 3, 0}, {0, 5, 0, 3},
         {0, 3, 5, 0}, {0, 3, 0, 5}, {0, 0, 5, 3}, {0, 0, 3, 5}, {5, 2, 1, 0}, {5, 2, 0, 1}, {5, 1, 2, 0}, {5, 1, 0, 2},
         {5, 0, 2, 1}, {5, 0, 1, 2}, {2, 5, 1, 0}, {2, 5, 0, 1}, {2, 1, 5, 0}, {2, 1, 0, 5}, {2, 0, 5, 1}, {2, 0, 1, 5},
         {1, 5, 2, 0}, {1, 5, 0, 2}, {1, 2, 5, 0}, {1, 2, 0, 5}, {1, 0, 5, 2}, {1, 0, 2, 5}, {0, 5, 2, 1}, {0, 5, 1, 2},
         {0, 2, 5, 1}, {0, 2, 1, 5}, {0, 1, 5, 2}, {0, 1, 2, 5}, {5, 1, 1, 1}, {1, 5, 1, 1}, {1, 1, 5, 1}, {1, 1, 1, 5},
         {4, 4, 0, 0}, {4, 0, 4, 0}, {4, 0, 0, 4}, {0, 4, 4, 0}, {0, 4, 0, 4}, {0, 0, 4, 4}, {4, 3, 1, 0}, {4, 3, 0, 1},
         {4, 1, 3, 0}, {4, 1, 0, 3}, {4, 0, 3, 1}, {4, 0, 1, 3}, {3, 4, 1, 0}, {3, 4, 0, 1}, {3, 1, 4, 0}, {3, 1, 0, 4},
         {3, 0, 4, 1}, {3, 0, 1, 4}, {1, 4, 3, 0}, {1, 4, 0, 3}, {1, 3, 4, 0}, {1, 3, 0, 4}, {1, 0, 4, 3}, {1, 0, 3, 4},
         {0, 4, 3, 1}, {0, 4, 1, 3}, {0, 3, 4, 1}, {0, 3, 1, 4}, {0, 1, 4, 3}, {0, 1, 3, 4}, {4, 2, 2, 0}, {4, 2, 0, 2},
         {4, 0, 2, 2}, {2, 4, 2, 0}, {2, 4, 0, 2}, {2, 2, 4, 0}, {2, 2, 0, 4}, {2, 0, 4, 2}, {2, 0, 2, 4}, {0, 4, 2, 2},
         {0, 2, 4, 2}, {0, 2, 2, 4}, {4, 2, 1, 1}, {4, 1, 2, 1}, {4, 1, 1, 2}, {2, 4, 1, 1}, {2, 1, 4, 1}, {2, 1, 1, 4},
         {1, 4, 2, 1}, {1, 4, 1, 2}, {1, 2, 4, 1}, {1, 2, 1, 4}, {1, 1, 4, 2}, {1, 1, 2, 4}, {3, 3, 2, 0}, {3, 3, 0, 2},
         {3, 2, 3, 0}, {3, 2, 0, 3}, {3, 0, 3, 2}, {3, 0, 2, 3}, {2, 3, 3, 0}, {2, 3, 0, 3}, {2, 0, 3, 3}, {0, 3, 3, 2},
         {0, 3, 2, 3}, {0, 2, 3, 3}, {3, 3, 1, 1}, {3, 1, 3, 1}, {3, 1, 1, 3}, {1, 3, 3, 1}, {1, 3, 1, 3}, {1, 1, 3, 3},
         {3, 2, 2, 1}, {3, 2, 1, 2}, {3, 1, 2, 2}, {2, 3, 2, 1}, {2, 3, 1, 2}, {2, 2, 3, 1}, {2, 2, 1, 3}, {2, 1, 3, 2},
         {2, 1, 2, 3}, {1, 3, 2, 2}, {1, 2, 3, 2}, {1, 2, 2, 3}, {2, 2, 2, 2}}};
    static constexpr auto nine_partitions_ = std::array<cell_partition, 216>{
        {{8, 1, 0, 0}, {8, 0, 1, 0}, {8, 0, 0, 1}, {1, 8, 0, 0}, {1, 0, 8, 0}, {1, 0, 0, 8}, {0, 8, 1, 0},
         {0, 8, 0, 1}, {0, 1, 8, 0}, {0, 1, 0, 8}, {0, 0, 8, 1}, {0, 0, 1, 8}, {7, 2, 0, 0}, {7, 0, 2, 0},
         {7, 0, 0, 2}, {2, 7, 0, 0}, {2, 0, 7, 0}, {2, 0, 0, 7}, {0, 7, 2, 0}, {0, 7, 0, 2}, {0, 2, 7, 0},
         {0, 2, 0, 7}, {0, 0, 7, 2}, {0, 0, 2, 7}, {7, 1, 1, 0}, {7, 1, 0, 1}, {7, 0, 1, 1}, {1, 7, 1, 0},
         {1, 7, 0, 1}, {1, 1, 7, 0}, {1, 1, 0, 7}, {1, 0, 7, 1}, {1, 0, 1, 7}, {0, 7, 1, 1}, {0, 1, 7, 1},
         {0, 1, 1, 7}, {6, 3, 0, 0}, {6, 0, 3, 0}, {6, 0, 0, 3}, {3, 6, 0, 0}, {3, 0, 6, 0}, {3, 0, 0, 6},
         {0, 6, 3, 0}, {0, 6, 0, 3}, {0, 3, 6, 0}, {0, 3, 0, 6}, {0, 0, 6, 3}, {0, 0, 3, 6}, {6, 2, 1, 0},
         {6, 2, 0, 1}, {6, 1, 2, 0}, {6, 1, 0, 2}, {6, 0, 2, 1}, {6, 0, 1, 2}, {2, 6, 1, 0}, {2, 6, 0, 1},
         {2, 1, 6, 0}, {2, 1, 0, 6}, {2, 0, 6, 1}, {2, 0, 1, 6}, {1, 6, 2, 0}, {1, 6, 0, 2}, {1, 2, 6, 0},
         {1, 2, 0, 6}, {1, 0, 6, 2}, {1, 0, 2, 6}, {0, 6, 2, 1}, {0, 6, 1, 2}, {0, 2, 6, 1}, {0, 2, 1, 6},
         {0, 1, 6, 2}, {0, 1, 2, 6}, {6, 1, 1, 1}, {1, 6, 1, 1}, {1, 1, 6, 1}, {1, 1, 1, 6}, {5, 4, 0, 0},
         {5, 0, 4, 0}, {5, 0, 0, 4}, {4, 5, 0, 0}, {4, 0, 5, 0}, {4, 0, 0, 5}, {0, 5, 4, 0}, {0, 5, 0, 4},
         {0, 4, 5, 0}, {0, 4, 0, 5}, {0, 0, 5, 4}, {0, 0, 4, 5}, {5, 3, 1, 0}, {5, 3, 0, 1}, {5, 1, 3, 0},
         {5, 1, 0, 3}, {5, 0, 3, 1}, {5, 0, 1, 3}, {3, 5, 1, 0}, {3, 5, 0, 1}, {3, 1, 5, 0}, {3, 1, 0, 5},
         {3, 0, 5, 1}, {3, 0, 1, 5}, {1, 5, 3, 0}, {1, 5, 0, 3}, {1, 3, 5, 0}, {1, 3, 0, 5}, {1, 0, 5, 3},
         {1, 0, 3, 5}, {0, 5, 3, 1}, {0, 5, 1, 3}, {0, 3, 5, 1}, {0, 3, 1, 5}, {0, 1, 5, 3}, {0, 1, 3, 5},
         {5, 2, 2, 0}, {5, 2, 0, 2}, {5, 0, 2, 2}, {2, 5, 2, 0}, {2, 5, 0, 2}, {2, 2, 5, 0}, {2, 2, 0, 5},
         {2, 0, 5, 2}, {2, 0, 2, 5}, {0, 5, 2, 2}, {0, 2, 5, 2}, {0, 2, 2, 5}, {5, 2, 1, 1}, {5, 1, 2, 1},
         {5, 1, 1, 2}, {2, 5, 1, 1}, {2, 1, 5, 1}, {2, 1, 1, 5}, {1, 5, 2, 1}, {1, 5, 1, 2}, {1, 2, 5, 1},
         {1, 2, 1, 5}, {1, 1, 5, 2}, {1, 1, 2, 5}, {4, 4, 1, 0}, {4, 4, 0, 1}, {4, 1, 4, 0}, {4, 1, 0, 4},
         {4, 0, 4, 1}, {4, 0, 1, 4}, {1, 4, 4, 0}, {1, 4, 0, 4}, {1, 0, 4, 4}, {0, 4, 4, 1}, {0, 4, 1, 4},
         {0, 1, 4, 4}, {4, 3, 2, 0}, {4, 3, 0, 2}, {4, 2, 3, 0}, {4, 2, 0, 3}, {4, 0, 3, 2}, {4, 0, 2, 3},
         {3, 4, 2, 0}, {3, 4, 0, 2}, {3, 2, 4, 0}, {3, 2, 0, 4}, {3, 0, 4, 2}, {3, 0, 2, 4}, {2, 4, 3, 0},
         {2, 4, 0, 3}, {2, 3, 4, 0}, {2, 3, 0, 4}, {2, 0, 4, 3}, {2, 0, 3, 4}, {0, 4, 3, 2}, {0, 4, 2, 3},
         {0, 3, 4, 2}, {0, 3, 2, 4}, {0, 2, 4, 3}, {0, 2, 3, 4}, {4, 3, 1, 1}, {4, 1, 3, 1}, {4, 1, 1, 3},
         {3, 4, 1, 1}, {3, 1, 4, 1}, {3, 1, 1, 4}, {1, 4, 3, 1}, {1, 4, 1, 3}, {1, 3, 4, 1}, {1, 3, 1, 4},
         {1, 1, 4, 3}, {1, 1, 3, 4}, {4, 2, 2, 1}, {4, 2, 1, 2}, {4, 1, 2, 2}, {2, 4, 2, 1}, {2, 4, 1, 2},
         {2, 2, 4, 1}, {2, 2, 1, 4}, {2, 1, 4, 2}, {2, 1, 2, 4}, {1, 4, 2, 2}, {1, 2, 4, 2}, {1, 2, 2, 4},
         {3, 3, 3, 0}, {3, 3, 0, 3}, {3, 0, 3, 3}, {0, 3, 3, 3}, {3, 3, 2, 1}, {3, 3, 1, 2}, {3, 2, 3, 1},
         {3, 2, 1, 3}, {3, 1, 3, 2}, {3, 1, 2, 3}, {2, 3, 3, 1}, {2, 3, 1, 3}, {2, 1, 3, 3}, {1, 3, 3, 2},
         {1, 3, 2, 3}, {1, 2, 3, 3}, {3, 2, 2, 2}, {2, 3, 2, 2}, {2, 2, 3, 2}, {2, 2, 2, 3}}};
    // clang-format on

    static constexpr auto cell_partitions_ = std::array<std::span<cell_partition const>, 10>{
        zero_partitions_, one_partitions_, two_partitions_,   three_partitions_, four_partitions_,
        five_partitions_, six_partitions_, seven_partitions_, eight_partitions_, nine_partitions_};
};


#endif // NUMBER_CROSS_CELL_PARTITIONS_H
