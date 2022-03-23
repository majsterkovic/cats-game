#include "uti.h"
#include "Cat.h"

#include <fstream>

sf::String playerInput;

bool on_element(sf::Vector2i mouse_position, sf::FloatRect bounds)
{
    if( (float)mouse_position.x > bounds.left && (float)mouse_position.x < bounds.left + bounds.width &&
        (float)mouse_position.y > bounds.top  && (float)mouse_position.y < bounds.top  + bounds.height)
    {
        return true;
    }
    return false;
}

void setup_window(sf::RenderWindow* window)
{
    window->create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Gra w Kotki", sf::Style::Close | sf::Style::Titlebar);
    window->setFramerateLimit(30);
    sf::Image image;
    image.loadFromFile( "laughing.png" );
    window->setIcon( image.getSize().x, image.getSize().y, image.getPixelsPtr() );
}

void highscores(sf::RenderWindow* window, std::string &player_time)
{
    sf::Font font, font1;
    sf::Text text[10];
    int fontsize = 38;
    std::vector<sf::FloatRect> bounds;
    std::vector<std::string> names;
    std::vector<int> times;

    font.loadFromFile("Meows-VGWjy.ttf");
    font1.loadFromFile("MeowsNepilRegular-Yzvwa.ttf");

    std::fstream scores;
    scores.open("scores.txt", std::ios::in | std::ios::out);
    std::string str;
    while (std::getline(scores, str))
    {
        if(!str.empty())
        {
            std::string name = str.substr(0, str.find(';'));
            std::string time = str.substr(str.find(';') + 1);
            names.push_back(name);
            times.push_back(std::stoi(time));
        }
    }
    scores.close();

    int j = 0;
    while(times[j] < std::stoi(player_time))
    {
        j++;
    }
    if(j < 10)
    {
        names.insert(names.begin() + j, playerInput);
        times.insert(times.begin() + j, std::stoi(player_time));
    }

    scores.open("scores.txt", std::ios::out | std::ios::trunc);
    for(int i = 0; i < names.size(); i++)
    {
        scores << names[i] << ";" << times[i] << std::endl;
    }
    scores.close();

    for(int i = 0; i < 10; i++)
    {
        text[i].setFont(font);
        if(i < 3)
        {
            text[i].setStyle(sf::Text::Bold);
        }
        text[i].setCharacterSize(fontsize);
        text[i].setFillColor(sf::Color::White);
        text[i].setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f, (float)(i + 1) * 48.0f + 40.0f));
        text[i].setString(std::to_string(i+1) + ". " + names[i] + " > " + std::to_string(times[i]) + "s");
        bounds.push_back(text[i].getLocalBounds());

        text[i].setOrigin(bounds[i].left + bounds[i].width/2.0f,
                          bounds[i].top  + bounds[i].height/2.0f);
    }
    sf::Text header;
    header.setFont(font1);
    header.setStyle(sf::Text::Underlined);
    header.setCharacterSize(fontsize + 2);
    header.setFillColor(sf::Color::White);
    header.setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f, 25.0f));
    header.setString("NAJLEPSZE WYNIKI");
    header.setOrigin(header.getLocalBounds().left + header.getLocalBounds().width/2.0f,
                     header.getLocalBounds().top  + header.getLocalBounds().height/2.0f);

    while (window->isOpen())
    {
        window->clear(sf::Color::Black);

        for(const auto& t : text)
        {
            window->draw(t);
        }
        window->draw(header);

        window->display();
        sf::Event event{};
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window->close();
            }
        }
    }
}

void display_score(sf::RenderWindow* window, int time, std::vector<Cat*>cats)
{
    sf::Font font2;

    font2.loadFromFile("Meows-VGWjy.ttf");

    sf::Text text;
    text.setFont(font2);

    int uncovered_cats = 0;
    for(int i = 0; i < cats.size(); i++)
    {
        if(cats[i]->uncovered)
        {
            uncovered_cats++;
        }
    }

    std::ostringstream time_to_display;
    time_to_display.precision(2);

    static bool win = false;
    static int t;

    if(!win)
    {
        t = time;
    }

    if(uncovered_cats == cats.size())
    {
        win = true;
    }

    time_to_display << std::fixed <<  ((float)t / 1000.0f);
    std::string time_string = time_to_display.str();
    if(win)
    {
        text.setString("Wygrana! Czas: " + time_string + "s");
    }
    else
    {
        text.setString("Czas: " + time_string + "s");
    }
    text.setCharacterSize(48);
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(4, WINDOW_HEIGHT - 56));
    window->draw(text);

    sf::Text text2;
    text2.setFont(font2);
    text2.setString("HIGHSCORES");
    text2.setCharacterSize(48);
    sf::FloatRect bounds = text2.getGlobalBounds();
    text2.setFillColor(sf::Color::White);
    text2.setPosition(sf::Vector2f(WINDOW_WIDTH - bounds.width - 8, WINDOW_HEIGHT - 56));
    bounds = text2.getGlobalBounds();

    if(win)
    {
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);

        if(on_element(mouse_pos, bounds))
        {
            text2.setFillColor(sf::Color(206,161,139));
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                // sleep 10 ns
                std::this_thread::sleep_for(std::chrono::nanoseconds(77));
                highscores(window, time_string);
            }
        }
        else
        {
            text2.setFillColor(sf::Color::White);
        }
        window->draw(text2);
    }
}

void handle_mouse_click(sf::Event &event, sf::RenderWindow* window, std::vector<Cat*> &cats, std::list<Cat*> &clicked_cats, int delta_time)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Right)
        {
            if(event.mouseButton.y > WINDOW_HEIGHT - 64)
            {
                return;
            }
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

    std::map<char, bool> easter_egg = {
            {'K', false},
            {'O', false},
            {'C', false},
            {'I', false},
            {'E', false},
    };

    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        display_score(&window, delta_time, cats);

        for(auto& cat : cats)
        {
            cat->draw(&window);
        }

        window.display();

        delta_time = clock.getElapsedTime().asMilliseconds();

        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            handle_mouse_click(event, &window, cats, clicked_cats, delta_time);
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::K))    easter_egg['K'] = true;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O))    easter_egg['O'] = true;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C))    easter_egg['C'] = true;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::I))    easter_egg['I'] = true;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))    easter_egg['E'] = true;

        if(std::all_of(easter_egg.begin(), easter_egg.end(), [](auto& pair) { return pair.second; }))
        {
            for (auto &cat: cats)
            {
                cat->egg = "egg/";
                cat->set_texture();
            }
            easter_egg['K'] = false;
        }

        for(auto& cat : clicked_cats)
        {
            if(cat->unclick(delta_time))
            {
                clicked_cats.erase(std::remove(clicked_cats.begin(), clicked_cats.end(), cat), clicked_cats.end());
            }
        }
    }
}

void start_menu(sf::RenderWindow &window)
{
    sf::Font font[2];
    sf::Text text[3];
    std::vector<sf::Vector2f> positions = {sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 192),
                                           sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 64),
                                           sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 192)};
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

    sf::String incentive = "Wpisz swoje imie: ";
    sf::Text playerText;

    playerText.setFont(font[1]);
    playerText.setCharacterSize(38);
    playerText.setFillColor(sf::Color::White);
    playerText.setPosition(sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f + 64));
    playerText.setString(incentive);
    sf::FloatRect playerText_bounds = playerText.getLocalBounds();

    playerText.setOrigin(playerText_bounds.left + playerText_bounds.width/2.0f,
                         playerText_bounds.top  + playerText_bounds.height/2.0f);


    while (window.isOpen())
    {
        window.clear(sf::Color::Black);
        sf::Vector2i mouse = sf::Mouse::getPosition(window);


        if(on_element(mouse, bounds_menu))
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

        for(const auto& t : text)
        {
            window.draw(t);
        }

        window.draw(rect);

        if( (sf::Keyboard::isKeyPressed(sf::Keyboard::SemiColon) && sf::Keyboard::isKeyPressed(sf::Keyboard::Multiply)) )
        {
            text[1].setString("KOTKU");
        }


        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode > 64 && event.text.unicode < 128 || event.text.unicode == 32)
                {
                    playerInput += static_cast<char>(event.text.unicode);
                    playerText.setString(incentive + playerInput);
                }
                else if(event.text.unicode == 8)
                {
                    if(playerInput.getSize() > 0)
                    {
                        playerInput.erase(playerInput.getSize() - 1, 1);
                        playerText.setString(incentive + playerInput);
                    }
                }
                playerText_bounds = playerText.getLocalBounds();
                playerText.setOrigin(playerText_bounds.left + playerText_bounds.width/2.0f,
                                     playerText_bounds.top  + playerText_bounds.height/2.0f);
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    if(on_element({event.mouseButton.x, event.mouseButton.y}, bounds_sound))
                    {
                        if(SOUND)
                        {
                            rect.setOutlineColor(sf::Color	(206,161,139));
                            rect.setOutlineThickness(2);
                            SOUND = false;
                        }
                        else
                        {
                            rect.setOutlineThickness(0);
                            SOUND = true;
                        }
                    }
                }
            }
        }

        window.draw(playerText);
        window.display();
    }

}

int main()
{
    sf::RenderWindow window;
    setup_window(&window);

    start_menu(window);
}
