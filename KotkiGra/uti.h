//
// Created by majsterkovic on 20.03.2022.
//

#ifndef KOTKIGRA_UTI_H
#define KOTKIGRA_UTI_H

#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <random>
#include <vector>
#include <algorithm>
#include <list>
#include <thread>
#include <sstream>
#include <chrono>

#define S 6
#define CATS 24

#define IMG_SIZE 128

#define WINDOW_WIDTH 768
#define WINDOW_HEIGHT (512 + 64)

inline bool SOUND = true;

std::pair <int, int> i_to_xy(int i);

int xy_to_i(int x, int y);

std::vector<int> get_random_sequence_with_double_occurences(int size);



#endif //KOTKIGRA_UTI_H
