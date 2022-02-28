#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <random>
#include <vector>
#include <algorithm>
#include <list>

#define W 4
#define S 6
#define CATS 24

#define IMG_SIZE 128

#define WINDOW_WIDTH 768
#define WINDOW_HEIGHT 512 + 64

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

class Cat
{

private:
    sf::Texture texture;
    sf::Texture texture_empty;
    sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(IMG_SIZE, IMG_SIZE));
    float time_clicked = 0;
    sf::SoundBuffer buffer;
    sf::Sound sound;

public:

    int number;
    int image_number;
    int x, y;
    bool uncovered = false;


    Cat(int i, int j)
    {
        number = i;
        image_number = j;
        x = i_to_xy(i).first;
        y = i_to_xy(i).second;
        Cat::set_texture();
        Cat::set_sprite();
        Cat::set_sound();
    }


    void set_texture()
    {
        texture.loadFromFile("cats/" + std::to_string(image_number)+ ".png");
        texture_empty.loadFromFile("cats/e.png");
    }

    void set_sprite()
    {
        rect.setSize(sf::Vector2f(IMG_SIZE, IMG_SIZE));
        rect.setTexture(&texture_empty);
        rect.setPosition((float)x*IMG_SIZE, (float)y*IMG_SIZE);
        rect.setOutlineThickness(3);
        rect.setOutlineColor(sf::Color::Black);
    }

    void set_sound()
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

    void draw(sf::RenderWindow* window)
    {
        window->draw(rect);
    }

    void show()
    {
        // play sound meow

        rect.setTexture(&texture);
        sound.play();
    }

    void hide()
    {
        rect.setTexture(&texture_empty);
    }

    void click(float time)
    {
        time_clicked = time;
        Cat::show();
    }

    bool unclick(float time, bool cnd = false)
    {
        if( (time - time_clicked > 1000) || cnd)
        {
            Cat::hide();
            time_clicked = 0;
            return true;
        }
        return false;
    }



};

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

void setup_window(sf::RenderWindow* window)
{
    window->create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Gra w Kotki", sf::Style::Close | sf::Style::Titlebar);
    window->setFramerateLimit(60);
    sf::Image image;
    image.loadFromFile( "laughing.png" );
    window->setIcon( image.getSize().x, image.getSize().y, image.getPixelsPtr() );
}

void display_score(sf::RenderWindow* window, int score)
{
    sf::Font font1;
    sf::Font font2;
    if (!font2.loadFromFile("Meows-VGWjy.ttf"))
    {
        std::cout << "Error loading font" << std::endl;
    }
    if (!font1.loadFromFile("MeowsNepilRegular-Yzvwa.ttf"))
    {
        std::cout << "Error loading font" << std::endl;
    }
    sf::Text text;
    text.setFont(font2);
    text.setString("Score: " + std::to_string(score));
    text.setCharacterSize(48);
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(0, WINDOW_HEIGHT - 56));
    window->draw(text);
}

int main()
{
    sf::RenderWindow window;
    setup_window(&window);

    sf::Clock clock;
    float delta_time = 0.0;

    std::vector<Cat*> cats;
    cats.reserve(CATS);
    std::vector<int> cat_images = get_random_sequence_with_double_occurences(CATS / 2);


    for(int i = 0; i < CATS; i++)
    {
        cats.push_back(new Cat(i, cat_images[i]));
    }

    std::list<Cat*> clicked_cats;

    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        display_score(&window, delta_time);

        for(auto& cat : cats)
        {
            //std::cout << cat->number << " " << cat->image_number << " " << cat->x << " " << cat->y << std::endl;
            cat->draw(&window);
        }

        window.display();

        delta_time = clock.getElapsedTime().asMilliseconds();
        //std::cout << delta_time << std::endl;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    int i = xy_to_i(event.mouseButton.x / IMG_SIZE, event.mouseButton.y / IMG_SIZE);
                    cats[i]->click(delta_time);
                    std::cout << "clicked cat: "<< cats[i]->number << std::endl;

                    if(clicked_cats.size() == 2)
                    {
                        // unclick first cat
                        clicked_cats.front()->unclick(0, true);
                        clicked_cats.pop_front();
                    }

                    if((std::find(clicked_cats.begin(), clicked_cats.end(), cats[i]) == clicked_cats.end()) && !cats[i]->uncovered)
                    {
                        clicked_cats.push_back(cats[i]);
                    }
                    if(clicked_cats.size() == 2)
                    {
                        if(clicked_cats.front()->image_number == clicked_cats.back()->image_number)
                        {
                            for(auto& cat : clicked_cats)
                            {
                                cat->uncovered = true;
                                clicked_cats.pop_front();
                            }
                        }
                    }



                }
            }
        }

        for(auto& cat : clicked_cats)
        {
            if(cat->unclick(delta_time))
            {
                std::cout << "unclicked cat: "<< cat->number << std::endl;
                clicked_cats.erase(std::remove(clicked_cats.begin(), clicked_cats.end(), cat), clicked_cats.end());
            }
        }


    }
}
