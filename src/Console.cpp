#include <u_console/Console.hpp>
#include <iostream>

namespace u_console {
Console::Console(unsigned int width, unsigned int height, const std::string& title)
    : m_window(sf::VideoMode({width, height}), title) {
    std::cout << "u_console initialized." << std::endl;

    if(!this->font.openFromFile("/usr/share/fonts/aajohan-comfortaa-fonts/Comfortaa-Regular.otf")){
        return;
    }

    this->vertical_bottom_offset = height;
}

void Console::run() {
    handleWindow();
}

void Console::run(const char* filePath) {
    handleWindow();
}

void Console::run(std::fstream& stream) {
    handleWindow();
}

void Console::set_message(const char* message, MessageType type){
    ComposedMessage* m = new ComposedMessage(message,this->font, this->FONT_SIZE, type);
    this->messages->push_back(m);
}

void Console::write(const char* message){
    set_message(message, NORMAL);
}

void Console::write_info(const char* message){
    set_message(message, INFO);
}

void Console::write_warning(const char* message){
    set_message(message, WARN);
}

void Console::write_error(const char* message){
    set_message(message, ERROR);
}

void Console::handleWindow(){
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);

    while (m_window.isOpen()) {

        if(!m_window.hasFocus()){
            m_window.display();
            continue;
        }

        while (const std::optional event = m_window.pollEvent()){
            if (event->is<sf::Event::Closed>()) {
                m_window.close();
            }

            if(event->is<sf::Event::MouseWheelScrolled>()){
                auto e = event->getIf<sf::Event::MouseWheelScrolled>();
                
                if( e->delta < 0 && vertical_top_offset <=0){
                    continue;
                }

                vertical_top_offset += e->delta * FONT_SIZE;
                vertical_bottom_offset += e->delta * FONT_SIZE;
            }
        }

        m_window.clear();
        
        int index = 0;
        for(auto m : *messages){
            
            auto& cpm = m->composed_message;
            cpm.setPosition({0, static_cast<float>(index * FONT_SIZE)});

            if(cpm.getPosition().y >= vertical_top_offset && cpm.getPosition().y <= vertical_bottom_offset){
                m->composed_message.setPosition({0.0f, static_cast<float>(cpm.getPosition().y - vertical_top_offset)});
                m_window.draw(cpm);
            }
            
            index++;
        }
        m_window.display();
    }

    delete messages;
}
}


ComposedMessage::ComposedMessage(const char* content, sf::Font& font, unsigned int font_size, MessageType type){
    this->content = content;
    this->type = type;
    this->composed_message.setString(content);
    this->composed_message.setCharacterSize(font_size);
    this->composed_message.setFont(font);

    this->font = &font;
    this->font_size = font_size;

    switch (type)
    {
    case ERROR:
        composed_message.setFillColor(sf::Color::Red);
        break;
    case WARN:
        composed_message.setFillColor(sf::Color::Yellow);
    case INFO:
        composed_message.setFillColor(sf::Color::Blue);
    default:
        break;
    }
}
