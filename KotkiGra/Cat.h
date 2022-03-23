//
// Created by majsterkovic on 20.03.2022.
//

#ifndef KOTKIGRA_CAT_H
#define KOTKIGRA_CAT_H

#include "uti.h"


class Cat
{

private:
    sf::Texture texture;
    sf::Texture texture_empty;
    sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(IMG_SIZE, IMG_SIZE));
    int time_clicked = 0;
    sf::SoundBuffer buffer;
    sf::Sound sound;

public:
    int number;
    int image_number;
    int x, y;
    bool uncovered = false;
    std::string egg;

    Cat(int i, int j);

    void set_texture();

    void set_sprite();

    void set_sound();

    void draw(sf::RenderWindow* window);

    void show();

    void hide();

    void click(int time);

    bool unclick(int time, bool cnd = false);

};


#endif //KOTKIGRA_CAT_H
