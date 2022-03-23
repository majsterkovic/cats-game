//
// Created by majsterkovic on 20.03.2022.
//

#include "uti.h"

std::pair <int, int> i_to_xy(int i)
{
    int y = i / S;
    int x = i % S;
    return std::make_pair(x, y);
}

int xy_to_i(int x, int y)
{
    return y * S + x;
}

std::vector<int> get_random_sequence_with_double_occurences(int size)
{
    std::vector<int> v;
    std::vector<int> v2;
    for(int i = 0; i < size; i++)
    {
        v.push_back(i);
        v2.push_back(i);
    }
    v.insert(v.end(), v2.begin(), v2.end());
    std::shuffle(v.begin(), v.end(), std::mt19937(std::random_device()()));
    return v;
}