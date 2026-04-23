#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace u_console {
class Console {
public:
    Console(unsigned int width, unsigned int height, const std::string& title);
    void run();

private:
    sf::RenderWindow m_window;
};
}
