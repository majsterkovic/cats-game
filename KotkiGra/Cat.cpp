//
// Created by majsterkovic on 20.03.2022.
//

#include "Cat.h"


Cat::Cat(int i, int j)
    {
        number = i;
        image_number = j;
        x = i_to_xy(i).first;
        y = i_to_xy(i).second;
        Cat::set_texture();
        Cat::set_sprite();
        Cat::set_sound();
    }

void Cat::set_texture()
    {
        texture.loadFromFile("cats/" + egg + std::to_string(image_number)+ ".png");
        texture.setSmooth(true);
        texture_empty.setSmooth(true);
        texture_empty.loadFromFile("cats/e.jpeg");
    }

void Cat::set_sprite()
    {
        rect.setSize(sf::Vector2f(IMG_SIZE, IMG_SIZE));
        rect.setTexture(&texture_empty);
        rect.setPosition((float)x*IMG_SIZE, (float)y*IMG_SIZE);
        rect.setOutlineThickness(3);
        rect.setOutlineColor(sf::Color::Black);
    }

void Cat::set_sound()
    {
        if (!buffer.loadFromFile("meow.wav"))
        {
            std::cout << "Error loading sound file" << std::endl;
        }
        else
        {
            sound.setBuffer(buffer);
        }
    }

void Cat::draw(sf::RenderWindow* window)
    {
        window->draw(rect);
    }

void Cat::show()
    {
        rect.setTexture(&texture, true);
        if(SOUND)
        {
            sound.play();
        }
    }

void Cat::hide()
    {
        rect.setTexture(&texture_empty, true);
    }

void Cat::click(int time)
    {
        time_clicked = time;
        Cat::show();
    }

bool Cat::unclick(int time, bool cnd)
    {
        if( (time - time_clicked > 1000) || cnd)
        {
            Cat::hide();
            time_clicked = 0;
            return true;
        }
        return false;
    }



