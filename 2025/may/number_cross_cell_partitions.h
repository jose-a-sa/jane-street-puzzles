#ifndef NUMBER_CROSS_PARTITIONS_H
#define NUMBER_CROSS_PARTITIONS_H

#include <array>
#include <span>

struct grid_cross_partition
{
    uint8_t left;
    uint8_t top;
    uint8_t right;
    uint8_t bottom;
};

struct grid_cross_partitions_generator
{
    template<int Digit>
    static constexpr auto compute()
    {
        constexpr size_t kElements = (Digit + 1) * (Digit + 2) * (Digit + 3) / 6;

        std::array<grid_cross_partition, kElements> res{};

        int const dig = Digit;

        size_t count = 0;
        for(int l = 0; l <= dig; ++l)
            for(int t = 0; t <= dig - l; ++t)
                for(int r = 0; r <= dig - l - t; ++r)
                {
                    grid_cross_partition& part = res[count];

                    part.left   = l;
                    part.top    = t;
                    part.right  = r;
                    part.bottom = dig - l - t - r;
                    ++count;
                }

        return res;
    }
};

struct grid_cross_partitions
{
    static constexpr auto& get(int number) { return cell_partitions_[number]; }

private:
    static constexpr auto zero_partitions_  = std::array<grid_cross_partition, 0>{};
    static constexpr auto one_partitions_   = grid_cross_partitions_generator::compute<1>();
    static constexpr auto two_partitions_   = grid_cross_partitions_generator::compute<2>();
    static constexpr auto three_partitions_ = grid_cross_partitions_generator::compute<3>();
    static constexpr auto four_partitions_  = grid_cross_partitions_generator::compute<4>();
    static constexpr auto five_partitions_  = grid_cross_partitions_generator::compute<5>();
    static constexpr auto six_partitions_   = grid_cross_partitions_generator::compute<6>();
    static constexpr auto seven_partitions_ = grid_cross_partitions_generator::compute<7>();
    static constexpr auto eight_partitions_ = grid_cross_partitions_generator::compute<8>();
    static constexpr auto nine_partitions_  = grid_cross_partitions_generator::compute<9>();

    static constexpr auto cell_partitions_ = std::array<std::span<grid_cross_partition const>, 10>{
        zero_partitions_, one_partitions_, two_partitions_,   three_partitions_, four_partitions_,
        five_partitions_, six_partitions_, seven_partitions_, eight_partitions_, nine_partitions_};
};


#endif // NUMBER_CROSS_PARTITIONS_H
