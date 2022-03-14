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

bool SOUND = true;

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
    int time_clicked = 0;
    sf::SoundBuffer buffer;
    sf::Sound sound;

public:
    int number;
    int image_number;
    int x, y;
    bool uncovered = false;
    std::string egg;

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
        texture.loadFromFile("cats/" + egg + std::to_string(image_number)+ ".png");
        texture.setSmooth(true);
        texture_empty.setSmooth(true);
        texture_empty.loadFromFile("cats/e.png");
    }

    void set_sprite()
    {
        rect.setSize(sf::Vector2f(IMG_SIZE, IMG_SIZE));
        rect.setTexture(&texture_empty);
        rect.setPosition((float)x*IMG_SIZE, (float)y*IMG_SIZE);
        //rect.setOutlineThickness(3);
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
        rect.setTexture(&texture, true);
        if(SOUND)
        {
            sound.play();
        }
        sound.play();
    }

    void hide()
    {
        rect.setTexture(&texture_empty, true);
    }

    void click(int time)
    {
        time_clicked = time;
        Cat::show();
    }

    bool unclick(int time, bool cnd = false)
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

void display_score(sf::RenderWindow* window, int time)
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

    std::ostringstream time_to_display;
    time_to_display.precision(2);
    time_to_display << std::fixed <<  (time / 1000.0f);
    std::string time_string = time_to_display.str();
    text.setString("Czas: " + time_string + "s");
    text.setCharacterSize(48);
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(4, WINDOW_HEIGHT - 56));
    window->draw(text);
}

void handle_mouse_click(sf::Event &event, sf::RenderWindow* window, std::vector<Cat*> &cats, std::list<Cat*> &clicked_cats, int delta_time)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Right)
        {
            int i = xy_to_i(event.mouseButton.x / IMG_SIZE, event.mouseButton.y / IMG_SIZE);
            cats[i]->click(delta_time);

            if (clicked_cats.size() == 2)
            {
                clicked_cats.front()->unclick(0, true);
                clicked_cats.pop_front();
            }

            if ((std::find(clicked_cats.begin(), clicked_cats.end(), cats[i]) == clicked_cats.end()) &&
                !cats[i]->uncovered)
            {
                clicked_cats.push_back(cats[i]);
            }

            if (clicked_cats.size() == 2)
            {
                if (clicked_cats.front()->image_number == clicked_cats.back()->image_number)
                {
                    for (auto &cat: clicked_cats)
                    {
                        cat->uncovered = true;
                        clicked_cats.pop_front();
                    }
                }
            }
        }
    }
}

void create_cats(std::vector<Cat*> &cats, std::vector<int> sequence)
{
    for (int i = 0; i < sequence.size(); i++)
    {
        cats.push_back(new Cat(i, sequence[i]));
    }
}

void game(sf::RenderWindow &window)
{
    std::vector<Cat*> cats;
    cats.reserve(CATS);

    create_cats(cats, get_random_sequence_with_double_occurences(CATS / 2));

    std::list<Cat*> clicked_cats;

    sf::Clock clock;
    int delta_time = 0;

    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        display_score(&window, delta_time);

        for(auto& cat : cats)
        {
            cat->draw(&window);
        }

        window.display();

        delta_time = clock.getElapsedTime().asMilliseconds();
        //std::cout << delta_time << std::endl;

        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            handle_mouse_click(event, &window, cats, clicked_cats, delta_time);

        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                for(auto& cat : cats)
                {
                    cat->egg = "egg/";
                    cat->set_texture();
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

void start_menu(sf::RenderWindow &window)
{
    sf::Font font[2];
    sf::Text text[3];
    std::vector<sf::Vector2f> positions = {sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 128),
                                           sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f ),
                                           sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 128)};
    std::vector<int> fontsizes = {64, 108, 64};
    std::vector<std::string> texts = {"Witaj w grze", "KOTKI", "Miau! (Graj)"};
    std::vector<sf::FloatRect> bounds;

    font[1].loadFromFile("Meows-VGWjy.ttf");
    font[0].loadFromFile("MeowsNepilRegular-Yzvwa.ttf");

    for(int i = 0; i < 3; i++)
    {
        text[i].setFont(font[(1+i) % 2]);
        text[i].setCharacterSize(fontsizes[i]);
        text[i].setFillColor(sf::Color::White);
        text[i].setPosition(positions[i]);
        text[i].setString(texts[i]);
        text[i].setFillColor(sf::Color::White);
        bounds.push_back(text[i].getLocalBounds());



        text[i].setOrigin(bounds[i].left + bounds[i].width/2.0f,
                          bounds[i].top  + bounds[i].height/2.0f);
    }

    sf::FloatRect bounds_menu = text[2].getGlobalBounds();

    float mute_snd_size = IMG_SIZE/2.0;
    sf::Texture texture;
    sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(mute_snd_size, mute_snd_size));
    texture.loadFromFile("mute.png");
    rect.setTexture(&texture);
    rect.setPosition(WINDOW_WIDTH - mute_snd_size - 8, WINDOW_HEIGHT - mute_snd_size - 8);

    sf::FloatRect bounds_sound = rect.getGlobalBounds();

    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        sf::Vector2i mouse_position = sf::Mouse::getPosition(window);

        if( (float)mouse_position.x > bounds_menu.left && (float)mouse_position.x < bounds_menu.left + bounds_menu.width &&
            (float)mouse_position.y > bounds_menu.top  && (float)mouse_position.y < bounds_menu.top  + bounds_menu.height)
        {
            text[2].setFillColor(sf::Color	(206,161,139));
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                // sleep 10 ns
                std::this_thread::sleep_for(std::chrono::nanoseconds(77));
                game(window);
            }
        }
        else
        {
            text[2].setFillColor(sf::Color::White);
        }

        if( (float)mouse_position.x > bounds_sound.left && (float)mouse_position.x < bounds_sound.left + bounds_sound.width &&
            (float)mouse_position.y > bounds_sound.top  && (float)mouse_position.y < bounds_sound.top  + bounds_sound.height)
        {
            rect.setFillColor(sf::Color(128,128,128));
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                rect.setOutlineColor(sf::Color	(206,161,139));
                rect.setOutlineThickness(2);

                std::this_thread::sleep_for(std::chrono::nanoseconds(77));
                game(window);
            }
        }
        else
        {
            rect.setFillColor(sf::Color::White);
        }

        for(const auto& t : text)
        {
            window.draw(t);
        }

        window.draw(rect);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::SemiColon) && sf::Keyboard::isKeyPressed(sf::Keyboard::Multiply))
        {
            text[1].setString("KOTKU");
        }

        window.display();
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
    }

}

int main()
{
    sf::RenderWindow window;
    setup_window(&window);

    start_menu(window);
    //game(window);


}
