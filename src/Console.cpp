#include <u_console/Console.hpp>
#include <iostream>

namespace u_console {
Console::Console(unsigned int width, unsigned int height, const std::string& title)
    : m_window(sf::VideoMode({width, height}), title) {
    std::cout << "u_console initialized." << std::endl;
}

void Console::run() {
    while (m_window.isOpen()) {
        while (const std::optional event = m_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                m_window.close();
            }
        }

        m_window.clear();
        // Drawing logic here
        m_window.display();
    }
}
}
